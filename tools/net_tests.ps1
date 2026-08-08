<#
    Automated multiplayer network tests for remc2.

    Runs a series of sessions - two instances, or three where the scenario needs a
    survivor after one node goes away - each disturbing the connection in a different
    way, and reports whether the nodes still agreed with each other.

    Every instance that was started is judged: it has to reach the game, exchange turns
    and report no wedged NCB.  Checking only the nodes that happened to keep talking is
    how a three-player run with one silent node used to be recorded as a pass.

    The game itself needs no interaction: --auto_test drives the menus (skips the intros,
    enters the network game, confirms the session and starts the level once both players
    are present) and --network_debug writes one log per instance.

    Usage:
        .\net_tests.ps1                       # all tests
        .\net_tests.ps1 -Only drop,delay      # selected tests
        .\net_tests.ps1 -List                 # just list what would run

    Requires two runnable folders, one per instance (the second is created by copying the
    first; large data folders are linked rather than copied).
#>

param(
    [ValidateSet("Debug","Release")]
    [string]   $Config   = "Debug",
    [string]   $GameDir,      # runnable folder of the first instance (has the game data)
    [string]   $GameDir2,     # second instance; created from the first if missing
    [string]   $GameDir3,     # third instance, for the scenarios that need three players
    [string]   $Exe,          # freshly built binary to test
    [int]      $ServerPort = 3030,
    [int]      $ClientData = 3032,
    [int]      $Client2Data= 3033,
    [string[]] $Only     = @(),
    [switch]   $List,
    [switch]   $KeepLogs
)

$ErrorActionPreference = "Stop"

# Everything is derived from where this script lives (<repo>\tools), so the tests run
# from any checkout.  Each path can still be overridden on the command line.
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $GameDir)  { $GameDir  = Join-Path $RepoRoot "x64\$Config" }
if (-not $GameDir2) { $GameDir2 = Join-Path $RepoRoot "x64\${Config}2" }
if (-not $GameDir3) { $GameDir3 = Join-Path $RepoRoot "x64\${Config}3" }
# The build writes the binary into the runnable folder itself.  remc2\x64\$Config is an
# older output location that can still hold a copy from weeks ago, and copying that over the
# fresh build is how a test ends up measuring the wrong binary - which it then reports as a
# failure of the code under test.
if (-not $Exe)      { $Exe      = Join-Path $RepoRoot "x64\$Config\remc2.exe" }

# ---------------------------------------------------------------------------
# Test catalogue.  'extraHost'/'extraClient' are appended to that instance's command
# line; 'runSeconds' is how long the session is left alone before it is judged.
# ---------------------------------------------------------------------------
# Note on what is and is not simulated: the link is TCP, so nothing can be lost,
# duplicated or delivered out of order - it is pointless to pretend otherwise.  What an
# unreliable line really costs the game is time: a lost segment is retransmitted and
# everything queued behind it waits, which the 'stall' scenarios reproduce.
$Tests = @(
    @{ name = "baseline";        desc = "undisturbed session";                extraHost=@();                        extraClient=@();                        runSeconds=60 }
    @{ name = "delay";           desc = "30 ms on every message";             extraHost=@("--net_delay","30");      extraClient=@("--net_delay","30");      runSeconds=60 }
    @{ name = "delay-client";    desc = "only the client is slowed down";     extraHost=@();                        extraClient=@("--net_delay","60");      runSeconds=60 }
    @{ name = "jitter";          desc = "latency varying by 0-150 ms";        extraHost=@("--net_jitter","150");    extraClient=@("--net_jitter","150");    runSeconds=60 }
    @{ name = "stall";           desc = "300 ms pause every 50th message";    extraHost=@("--net_stall","300","--net_stall_every","50");
                                                                              extraClient=@("--net_stall","300","--net_stall_every","50");                 runSeconds=60 }
    @{ name = "stall-heavy";     desc = "800 ms pause every 15th message";    extraHost=@("--net_stall","800","--net_stall_every","15");
                                                                              extraClient=@("--net_stall","800","--net_stall_every","15");                 runSeconds=60 }
    @{ name = "mixed";           desc = "latency, jitter and stalls";         extraHost=@("--net_delay","20","--net_jitter","80","--net_stall","400","--net_stall_every","40");
                                                                              extraClient=@("--net_delay","20","--net_jitter","80","--net_stall","400","--net_stall_every","40"); runSeconds=60 }
    # The client must not run ahead while it is waiting for the host: a long pause in the
    # host's replies has to hold the client back, not let it simulate extra turns.
    @{ name = "host-freeze";     desc = "host unresponsive for 3 s at a time";extraHost=@();                        extraClient=@("--net_stall","3000","--net_stall_every","40"); runSeconds=60; checkLockstep=$true }
    @{ name = "host-freeze-long";desc = "host unresponsive for 8 s at a time";extraHost=@();                        extraClient=@("--net_stall","8000","--net_stall_every","25"); runSeconds=60; checkLockstep=$true }
    @{ name = "client-quits";    desc = "client leaves, host must carry on";  extraHost=@();                        extraClient=@("--quit_after","20");     runSeconds=60; expect="hostSurvives" }
    @{ name = "host-quits";      desc = "host leaves, client must carry on";  extraHost=@("--quit_after","20");     extraClient=@();                        runSeconds=60; expect="clientSurvives" }
    @{ name = "client-vanishes"; desc = "client is killed outright";          extraHost=@();                        extraClient=@();                        runSeconds=60; killAfter="client" }
    @{ name = "host-vanishes";   desc = "host is killed outright";            extraHost=@();                        extraClient=@();                        runSeconds=60; killAfter="host" }
    @{ name = "link-lost";       desc = "client tears its own link down";     extraHost=@();                        extraClient=@("--net_kill_after","20"); runSeconds=60 }

    # Three players.  With two, losing one leaves nobody to carry on with, so these are the
    # scenarios that say whether the session survives a node going away - and, when the node
    # that goes is the server, whether somebody else takes the role over.
    #
    # The undisturbed three-player session comes first: until it holds, none of the
    # kill scenarios below mean anything.  They used to "pass" with the third node
    # wedged in a RECEIVE that never completed, because only the two nodes that did
    # exchange turns were ever looked at.
    @{ name = "baseline-3p";        desc = "3p: undisturbed session";              players=3; runSeconds=60 }
    @{ name = "client-dies-lobby";  desc = "3p: a client is killed in the lobby";  players=3; killWhen="lobby"; killTarget="client2"; waitPlayers=2; runSeconds=70; expect="restStartLevel" }
    @{ name = "client-dies-game";   desc = "3p: a client is killed in the game";   players=3; killWhen="game";  killTarget="client2"; waitPlayers=3; runSeconds=70; expect="restKeepPlaying" }
    @{ name = "server-dies-lobby";  desc = "3p: the server is killed in the lobby";players=3; killWhen="lobby"; killTarget="host";    waitPlayers=2; runSeconds=70; expect="restStartLevel" }
    @{ name = "server-dies-game";   desc = "3p: the server is killed in the game"; players=3; killWhen="game";  killTarget="host";    waitPlayers=3; runSeconds=70; expect="restKeepPlaying" }
)

if ($List) {
    $Tests | ForEach-Object { "{0,-16} {1}" -f $_.name, $_.desc }
    return
}

if ($Only.Count -gt 0) {
    # Called through -File, "a,b" arrives as one string, so split it apart again.
    $wanted = $Only | ForEach-Object { $_ -split ',' } | Where-Object { $_ -ne '' }
    $Tests = @($Tests | Where-Object { $wanted -contains $_.name })
    if ($Tests.Count -eq 0) { throw "No test matches: $($wanted -join ', ')" }
}

# ---------------------------------------------------------------------------
function Stop-AllInstances {
    Get-Process remc2 -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep -Seconds 2
}

function Initialize-InstanceFolder([string]$target) {
    # Mirror the runnable folder; the bulky read-only data is linked, not duplicated.
    if (Test-Path (Join-Path $target "remc2.exe")) { return }
    Write-Host "Preparing instance folder $target ..." -ForegroundColor DarkGray
    New-Item -ItemType Directory -Path $target -Force | Out-Null
    $linked = @('NETHERW','CD_Files','fixed-ingame-graphics','font','graphics','memimages','music-ogg','kiss')
    foreach ($item in Get-ChildItem $GameDir) {
        $dest = Join-Path $target $item.Name
        if ($item.PSIsContainer) {
            if ($linked -contains $item.Name) { cmd /c "mklink /J `"$dest`" `"$($item.FullName)`"" | Out-Null }
            else { Copy-Item $item.FullName $dest -Recurse -Force }
        }
        elseif ($item.Name -notlike 'net_messages_log1_*') { Copy-Item $item.FullName $dest -Force }
    }
}

# Longest pause a scenario can deliberately impose on one message, so the judgement can
# tell an injected wait apart from a genuinely wedged NCB.
function Get-InjectedHoldMs($argList) {
    $d = 0; $j = 0; $s = 0
    if (-not $argList) { return 0 }
    for ($i = 0; $i -lt $argList.Count - 1; $i++) {
        switch ($argList[$i]) {
            '--net_delay'  { $d = [int]$argList[$i + 1] }
            '--net_jitter' { $j = [int]$argList[$i + 1] }
            '--net_stall'  { $s = [int]$argList[$i + 1] }
        }
    }
    return $d + $j + $s
}

function Get-NetLog([string]$folder, [int]$port) {
    Join-Path $folder ("net_messages_log1_{0}.txt" -f $port)
}

function Measure-Session([string]$logPath, [int]$sinceMs = -1) {
    # Reads one instance's log and reports what the network layer actually did.
    $result = [ordered]@{
        reachedGame = $false; sent = 0; queued = 0; delivered = 0
        backlog = 0; maxQueue = 0; stalled = 0
        disconnects = 0; fatal = 0; stuck = 0
        deliveredAfterKill = 0; tookOver = $false; startedLevel = $false
    }
    if (-not (Test-Path $logPath)) { return $result }
    $text = Get-Content $logPath -Raw
    $result.reachedGame = $text -match 'host starts the level|level selection reached'
    $result.sent        = ([regex]::Matches($text, 'SendNetwork: sent')).Count
    $result.queued      = ([regex]::Matches($text, 'queued DIRECT_SEND')).Count
    $result.delivered   = ([regex]::Matches($text, 'RECEIVE.*DELIVERED')).Count
    $result.stalled     = ([regex]::Matches($text, 'NETFAULT: stalling')).Count
    $result.disconnects = ([regex]::Matches($text, 'disconnected')).Count
    $result.fatal       = ([regex]::Matches($text, 'FATAL')).Count
    # An NCB the game is still spinning on.  This is what a wedged node looks like from
    # the outside, and it has to fail the scenario rather than pass quietly.
    $result.stuck       = ([regex]::Matches($text, 'STUCK')).Count
    $result.backlog     = $result.queued - $result.delivered
    $queues = [regex]::Matches($text, 'queue=(\d+)') | ForEach-Object { [int]$_.Groups[1].Value }
    if ($queues.Count -gt 0) { $result.maxQueue = ($queues | Measure-Object -Maximum).Maximum }
    $result.tookOver    = $text -match 'TAKEOVER'
    $result.startedLevel = $text -match 'host starts the level'

    # Every log line starts with milliseconds since that instance started, so "what happened
    # after the node was killed" can be counted without a second log.
    if ($sinceMs -ge 0) {
        foreach ($m in [regex]::Matches($text, '(?m)^(\d+)\|.*(RECEIVE.*DELIVERED|SendNetwork: sent)')) {
            if ([int]$m.Groups[1].Value -ge $sinceMs) { $result.deliveredAfterKill++ }
        }
    }
    return $result
}

function Invoke-Test($test) {
    Stop-AllInstances
    $players = if ($test.players) { [int]$test.players } else { 2 }
    $dirs = @($GameDir, $GameDir2); if ($players -ge 3) { $dirs += $GameDir3 }
    foreach ($d in $dirs) {
        $dest = Join-Path $d "remc2.exe"
        if ((Resolve-Path $Exe).Path -ne $dest) { Copy-Item $Exe $dest -Force }
        Get-ChildItem $d -Filter 'net_messages_log1_*.txt' -ErrorAction SilentlyContinue |
            Remove-Item -Force -ErrorAction SilentlyContinue
    }

    # The host waits for this many players before it starts the level.  A lobby scenario
    # holds the game there by asking for one more than will ever arrive.
    $wait = if ($test.waitPlayers) { [int]$test.waitPlayers } else { $players }

    # Not every scenario adds arguments; an absent entry must not become a null in the list.
    $extraHost   = if ($test.extraHost)   { $test.extraHost }   else { @() }
    $extraClient = if ($test.extraClient) { $test.extraClient } else { @() }

    $common     = @('--network','--network_debug','--auto_test','--auto_test_players',"$wait")
    $hostArgs   = $common + @('server', "$ServerPort")                              + $extraHost
    $clientArgs = $common + @('client', '127.0.0.1', "$ServerPort", "$ClientData")  + $extraClient
    $client2Args= $common + @('client', '127.0.0.1', "$ServerPort", "$Client2Data")

    $procs = @{}
    $started = @{}
    $started.host = Get-Date
    $procs.host   = Start-Process -FilePath (Join-Path $GameDir  "remc2.exe") -ArgumentList $hostArgs   -WorkingDirectory $GameDir  -PassThru
    Start-Sleep -Seconds 2
    $started.client = Get-Date
    $procs.client = Start-Process -FilePath (Join-Path $GameDir2 "remc2.exe") -ArgumentList $clientArgs -WorkingDirectory $GameDir2 -PassThru
    if ($players -ge 3) {
        Start-Sleep -Seconds 2
        $started.client2 = Get-Date
        $procs.client2 = Start-Process -FilePath (Join-Path $GameDir3 "remc2.exe") -ArgumentList $client2Args -WorkingDirectory $GameDir3 -PassThru
    }

    # A scenario may pull the plug on one node: either while the game is still in the level
    # selection, or once it is running.
    if ($test.killWhen -or $test.killAfter) {
        $target = if ($test.killTarget) { $test.killTarget } else { $test.killAfter }
        $when   = switch ($test.killWhen) {
            'lobby' { 12 }                                  # menus are through, level not started
            'game'  { 30 }                                  # level has been running a while
            default { [int]($test.runSeconds / 2) }
        }
        Start-Sleep -Seconds $when
        $killedAt = Get-Date
        $victim = $procs[$target]
        if ($victim -and -not $victim.HasExited) { Stop-Process -Id $victim.Id -Force }
        Start-Sleep -Seconds ([math]::Max(10, $test.runSeconds - $when))
    }
    else {
        Start-Sleep -Seconds $test.runSeconds
    }

    $alive = @{}
    foreach ($k in $procs.Keys) { $alive[$k] = -not $procs[$k].HasExited }
    foreach ($k in $procs.Keys) { if (-not $procs[$k].HasExited) { Stop-Process -Id $procs[$k].Id -Force } }
    Start-Sleep -Seconds 1

    # Offset of the kill within each instance's own log timeline.
    function KillOffset([string]$who) {
        if (-not $killedAt -or -not $started[$who]) { return -1 }
        return [int]((New-TimeSpan -Start $started[$who] -End $killedAt).TotalMilliseconds)
    }
    $h  = Measure-Session (Get-NetLog $GameDir  $ServerPort)  (KillOffset 'host')
    $c  = Measure-Session (Get-NetLog $GameDir2 $ClientData)  (KillOffset 'client')
    $c2 = if ($players -ge 3) { Measure-Session (Get-NetLog $GameDir3 $Client2Data) (KillOffset 'client2') } else { $null }

    # ---- judgement -------------------------------------------------------
    $problems = @()
    $killed   = if ($test.killTarget) { $test.killTarget } else { $test.killAfter }
    $survivors = @{}
    foreach ($k in @('host','client','client2')) {
        if ($procs.ContainsKey($k) -and $k -ne $killed) { $survivors[$k] = $true }
    }
    $stats = @{ host = $h; client = $c; client2 = $c2 }

    # True where a node is meant to go away, or tear its own link down, part way through.
    $leavesEarly = $test.expect -or $test.killAfter -or $test.killWhen -or ($extraClient -contains '--net_kill_after')

    # Judge every instance that was started, not just the host and the first client.  The
    # third node used to be looked at only through 'Injected', so a session where it never
    # got into the game - or sat wedged in a RECEIVE - still counted as a pass.
    # A wedged NCB is only meaningful where nothing was supposed to hold the game up.
    # WaitForNcb_diag reports STUCK after 3 s, so a scenario that injects a longer pause
    # trips it on purpose, and so does a killed peer - that is only noticed once the 5 s
    # heartbeat timeout expires.  Demanding zero reports there would fail the scenarios
    # for doing exactly what they were written to do.
    $maxHold = [math]::Max((Get-InjectedHoldMs $extraHost), (Get-InjectedHoldMs $extraClient))
    $checkStuck = (-not $leavesEarly) -and ($maxHold -lt 2000)

    foreach ($k in @('host','client','client2')) {
        if (-not $procs.ContainsKey($k)) { continue }
        $s = $stats[$k]; if (-not $s) { continue }
        if (-not $s.reachedGame)  { $problems += "$k did not reach the game" }
        if ($s.fatal -gt 0)       { $problems += "FATAL in $k log" }
        if ($checkStuck -and $s.stuck -gt 0) { $problems += "$k has $($s.stuck) stuck NCB report(s)" }
        if ($s.backlog -gt 5)     { $problems += "$k is $($s.backlog) messages behind" }
    }

    # A session that stops exchanging has frozen, which a backlog check alone will not
    # catch: if nothing is received, nothing piles up either.  Scale the expectation with
    # the time the session was actually given, and skip it where a side leaves by design.
    if (-not $leavesEarly) {
        $minExchanges = [int]($test.runSeconds * 2)   # deliberately generous
        foreach ($k in @('host','client','client2')) {
            if (-not $procs.ContainsKey($k)) { continue }
            $s = $stats[$k]; if (-not $s) { continue }
            if ($s.delivered -lt $minExchanges) { $problems += "$k stalled ($($s.delivered) exchanges)" }
        }
    }

    # Lockstep: neither side may take turns the other has not seen.  Each exchange is one
    # turn, so the two counts have to stay close no matter how slow the link gets.
    if ($test.checkLockstep) {
        $lead = [math]::Abs($h.sent - $c.sent)
        $worst = [math]::Max($h.sent, $c.sent)
        if ($worst -gt 0 -and $lead -gt [math]::Max(5, $worst * 0.1)) {
            $problems += "one side ran $lead turns ahead (host $($h.sent), client $($c.sent))"
        }
    }

    switch ($test.expect) {
        'hostSurvives'   { if (-not $alive.host)   { $problems += "host did not survive the client leaving" } }
        'clientSurvives' { if (-not $alive.client) { $problems += "client did not survive the host leaving" } }
        # Whoever was not killed has to still be running, and the ones that are left have to
        # get themselves into the level - which after the server was killed means one of them
        # took the role over.
        'restStartLevel' {
            foreach ($k in $survivors.Keys) {
                if (-not $alive[$k]) { $problems += "$k died with the $killed" }
            }
            # Reaching the level selection is not enough - somebody has to actually start the
            # level afterwards, which after the server was killed means one of the survivors
            # took the role over.
            $anyStarted = $false
            foreach ($k in $survivors.Keys) { if ($stats[$k] -and $stats[$k].startedLevel) { $anyStarted = $true } }
            if (-not $anyStarted) { $problems += "nobody started the level after the $killed went away" }
            foreach ($k in $survivors.Keys) {
                if ($alive[$k] -and $stats[$k].deliveredAfterKill -lt 10) {
                    $problems += "$k exchanged nothing after the $killed was killed"
                }
            }
        }
        # ...and here the survivors have to still be exchanging turns afterwards.
        'restKeepPlaying' {
            foreach ($k in $survivors.Keys) {
                if (-not $alive[$k]) { $problems += "$k died with the $killed" }
                elseif ($stats[$k].deliveredAfterKill -lt 20) {
                    $problems += "$k stopped exchanging after the $killed was killed ($($stats[$k].deliveredAfterKill))"
                }
            }
        }
    }
    # A side that was killed must not leave the other one stuck.
    if ($test.killAfter -eq 'host'   -and -not $alive.client) { $problems += "client died with the host" }
    if ($test.killAfter -eq 'client' -and -not $alive.host)   { $problems += "host died with the client" }

    return [pscustomobject]@{
        Name     = $test.name
        Result   = if ($problems.Count -eq 0) { "PASS" } else { "FAIL" }
        Detail   = if ($problems.Count -eq 0) { "" } else { $problems -join "; " }
        HostQ    = "$($h.delivered)/$($h.queued) maxQ=$($h.maxQueue)"
        ClientQ  = "$($c.delivered)/$($c.queued) maxQ=$($c.maxQueue)"
        Injected = if ($c2) { "3rd: $($c2.delivered)/$($c2.queued)" } else { "stalls: $($h.stalled + $c.stalled)" }
    }
}

# ---------------------------------------------------------------------------
if (-not (Test-Path $Exe)) {
    throw "Game binary not found: $Exe`nBuild the $Config configuration first, or pass -Exe <path>."
}
if (-not (Test-Path (Join-Path $GameDir "NETHERW"))) {
    throw "Game data not found in $GameDir`nPass -GameDir <folder containing NETHERW>."
}
Initialize-InstanceFolder $GameDir2
if ($Tests | Where-Object { $_.players -eq 3 }) { Initialize-InstanceFolder $GameDir3 }

Write-Host ""
Write-Host "remc2 network tests - $($Tests.Count) scenario(s), $Config build" -ForegroundColor Cyan
Write-Host "  binary:   $Exe"
Write-Host "  instance: $GameDir"
Write-Host "  instance: $GameDir2"
Write-Host ""

$results = @()
foreach ($test in $Tests) {
    Write-Host ("  {0,-16} {1}" -f $test.name, $test.desc) -NoNewline
    $r = Invoke-Test $test
    $results += $r
    if ($r.Result -eq "PASS") { Write-Host "  PASS" -ForegroundColor Green }
    else                      { Write-Host "  FAIL" -ForegroundColor Red; Write-Host "        $($r.Detail)" -ForegroundColor Yellow }
}

Stop-AllInstances

Write-Host ""
$results | Format-Table Name, Result, HostQ, ClientQ, Injected, Detail -AutoSize
# @() around the filter: with exactly one match Where-Object returns a bare object whose
# .Count is empty, so a single failing scenario reported "" failures and exited 0 - the run
# looked green precisely when one thing was broken.
$failed = @($results | Where-Object Result -eq "FAIL").Count
Write-Host ""
if ($failed -eq 0) { Write-Host "All $($results.Count) scenarios passed." -ForegroundColor Green }
else               { Write-Host "$failed of $($results.Count) scenarios failed." -ForegroundColor Red }
Write-Host "Per-instance logs:"
Write-Host "  $GameDir\net_messages_log1_$ServerPort.txt"
Write-Host "  $GameDir2\net_messages_log1_$ClientData.txt"
Write-Host "  $GameDir3\net_messages_log1_$Client2Data.txt   (three-player scenarios)"
exit $failed
