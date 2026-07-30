<#
    Automated multiplayer network tests for remc2.

    Runs a series of two-instance sessions, each one disturbing the connection in a
    different way, and reports whether the two nodes still agreed with each other.

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
    [string]   $Exe,          # freshly built binary to test
    [int]      $ServerPort = 3030,
    [int]      $HostData   = 3031,
    [int]      $ClientData = 3032,
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
if (-not $Exe)      { $Exe      = Join-Path $RepoRoot "remc2\x64\$Config\remc2.exe" }

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
    @{ name = "client-quits";    desc = "client leaves, host must carry on";  extraHost=@();                        extraClient=@("--quit_after","20");     runSeconds=60; expect="hostSurvives" }
    @{ name = "host-quits";      desc = "host leaves, client must carry on";  extraHost=@("--quit_after","20");     extraClient=@();                        runSeconds=60; expect="clientSurvives" }
    @{ name = "client-vanishes"; desc = "client is killed outright";          extraHost=@();                        extraClient=@();                        runSeconds=60; killAfter="client" }
    @{ name = "host-vanishes";   desc = "host is killed outright";            extraHost=@();                        extraClient=@();                        runSeconds=60; killAfter="host" }
    @{ name = "link-lost";       desc = "client tears its own link down";     extraHost=@();                        extraClient=@("--net_kill_after","20"); runSeconds=60 }
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

function Initialize-SecondFolder {
    # Mirror the runnable folder; the bulky read-only data is linked, not duplicated.
    if (Test-Path (Join-Path $GameDir2 "remc2.exe")) { return }
    Write-Host "Preparing second instance folder..." -ForegroundColor DarkGray
    New-Item -ItemType Directory -Path $GameDir2 -Force | Out-Null
    $linked = @('NETHERW','CD_Files','fixed-ingame-graphics','font','graphics','memimages','music-ogg','kiss')
    foreach ($item in Get-ChildItem $GameDir) {
        $target = Join-Path $GameDir2 $item.Name
        if ($item.PSIsContainer) {
            if ($linked -contains $item.Name) { cmd /c "mklink /J `"$target`" `"$($item.FullName)`"" | Out-Null }
            else { Copy-Item $item.FullName $target -Recurse -Force }
        }
        elseif ($item.Name -notlike 'net_messages_log1_*') { Copy-Item $item.FullName $target -Force }
    }
}

function Get-NetLog([string]$folder, [int]$port) {
    Join-Path $folder ("net_messages_log1_{0}.txt" -f $port)
}

function Measure-Session([string]$logPath) {
    # Reads one instance's log and reports what the network layer actually did.
    $result = [ordered]@{
        reachedGame = $false; sent = 0; queued = 0; delivered = 0
        backlog = 0; maxQueue = 0; stalled = 0
        disconnects = 0; fatal = 0
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
    $result.backlog     = $result.queued - $result.delivered
    $queues = [regex]::Matches($text, 'queue=(\d+)') | ForEach-Object { [int]$_.Groups[1].Value }
    if ($queues.Count -gt 0) { $result.maxQueue = ($queues | Measure-Object -Maximum).Maximum }
    return $result
}

function Invoke-Test($test) {
    Stop-AllInstances
    Copy-Item $Exe (Join-Path $GameDir  "remc2.exe") -Force
    Copy-Item $Exe (Join-Path $GameDir2 "remc2.exe") -Force
    Get-ChildItem $GameDir, $GameDir2 -Filter 'net_messages_log1_*.txt' -ErrorAction SilentlyContinue |
        Remove-Item -Force -ErrorAction SilentlyContinue

    $common = @('--network','--network_debug','--auto_test')
    $hostArgs   = $common + @('server', "$ServerPort", 'client', '127.0.0.1', "$ServerPort", "$HostData")   + $test.extraHost
    $clientArgs = $common + @('client', '127.0.0.1', "$ServerPort", "$ClientData")                          + $test.extraClient

    $hostProc = Start-Process -FilePath (Join-Path $GameDir "remc2.exe")  -ArgumentList $hostArgs   -WorkingDirectory $GameDir  -PassThru
    Start-Sleep -Seconds 2
    $clientProc = Start-Process -FilePath (Join-Path $GameDir2 "remc2.exe") -ArgumentList $clientArgs -WorkingDirectory $GameDir2 -PassThru

    # A test may pull the plug on one side half way through.
    if ($test.killAfter) {
        Start-Sleep -Seconds ([int]($test.runSeconds / 2))
        if ($test.killAfter -eq 'host'   -and -not $hostProc.HasExited)   { Stop-Process -Id $hostProc.Id   -Force }
        if ($test.killAfter -eq 'client' -and -not $clientProc.HasExited) { Stop-Process -Id $clientProc.Id -Force }
        Start-Sleep -Seconds ([int]($test.runSeconds / 2))
    }
    else {
        Start-Sleep -Seconds $test.runSeconds
    }

    $hostAlive   = -not $hostProc.HasExited
    $clientAlive = -not $clientProc.HasExited
    foreach ($p in @($hostProc, $clientProc)) { if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force } }
    Start-Sleep -Seconds 1

    $h = Measure-Session (Get-NetLog $GameDir  $HostData)
    $c = Measure-Session (Get-NetLog $GameDir2 $ClientData)

    # ---- judgement -------------------------------------------------------
    $problems = @()
    if (-not $h.reachedGame -or -not $c.reachedGame) { $problems += "did not reach the game" }
    if ($h.fatal -gt 0 -or $c.fatal -gt 0)           { $problems += "FATAL in log" }
    if ($h.backlog -gt 5)                            { $problems += "host is $($h.backlog) messages behind" }
    if ($c.backlog -gt 5)                            { $problems += "client is $($c.backlog) messages behind" }

    # A session that stops exchanging has frozen, which a backlog check alone will not
    # catch: if nothing is received, nothing piles up either.  Scale the expectation with
    # the time the session was actually given, and skip it where a side leaves by design.
    $leavesEarly = $test.expect -or $test.killAfter -or ($test.extraClient -contains '--net_kill_after')
    if (-not $leavesEarly) {
        $minExchanges = [int]($test.runSeconds * 2)   # deliberately generous
        if ($h.delivered -lt $minExchanges) { $problems += "host stalled ($($h.delivered) exchanges)" }
        if ($c.delivered -lt $minExchanges) { $problems += "client stalled ($($c.delivered) exchanges)" }
    }

    switch ($test.expect) {
        'hostSurvives'   { if (-not $hostAlive)   { $problems += "host did not survive the client leaving" } }
        'clientSurvives' { if (-not $clientAlive) { $problems += "client did not survive the host leaving" } }
    }
    # A side that was killed must not leave the other one stuck.
    if ($test.killAfter -eq 'host'   -and -not $clientAlive) { $problems += "client died with the host" }
    if ($test.killAfter -eq 'client' -and -not $hostAlive)   { $problems += "host died with the client" }

    if (-not $KeepLogs) { } # logs are left in place either way; kept for inspection

    return [pscustomobject]@{
        Name     = $test.name
        Result   = if ($problems.Count -eq 0) { "PASS" } else { "FAIL" }
        Detail   = if ($problems.Count -eq 0) { "" } else { $problems -join "; " }
        HostQ    = "$($h.delivered)/$($h.queued) maxQ=$($h.maxQueue)"
        ClientQ  = "$($c.delivered)/$($c.queued) maxQ=$($c.maxQueue)"
        Injected = "stalls: $($h.stalled + $c.stalled)"
    }
}

# ---------------------------------------------------------------------------
if (-not (Test-Path $Exe)) {
    throw "Game binary not found: $Exe`nBuild the $Config configuration first, or pass -Exe <path>."
}
if (-not (Test-Path (Join-Path $GameDir "NETHERW"))) {
    throw "Game data not found in $GameDir`nPass -GameDir <folder containing NETHERW>."
}
Initialize-SecondFolder

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
$failed = ($results | Where-Object Result -eq "FAIL").Count
Write-Host ""
if ($failed -eq 0) { Write-Host "All $($results.Count) scenarios passed." -ForegroundColor Green }
else               { Write-Host "$failed of $($results.Count) scenarios failed." -ForegroundColor Red }
Write-Host "Per-instance logs: $GameDir\net_messages_log1_$HostData.txt, $GameDir2\net_messages_log1_$ClientData.txt"
exit $failed
