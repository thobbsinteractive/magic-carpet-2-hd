$REMC2_EXECUTABLE = "$PSScriptRoot/../Release/remc2.exe"
$CONFIG_FILE_PATH = "$PSScriptRoot/renderer-config.ini"

function Test-Level {
    param (
        [int]$level
    )
    Write-Output "Testing renderer in level $level"
	Write-Output $REMC2_EXECUTABLE
	Write-Output $CONFIG_FILE_PATH
	$process = Start-Process -Wait -PassThru -FilePath $REMC2_EXECUTABLE -ArgumentList "--test_renderers --set_level $level --config_file_path $CONFIG_FILE_PATH"
	$result = $process.ExitCode
	
    if ($result -eq 0) {
        Write-Output "test level $level ok"
    } else {
        Write-Output "test level $level failed"
		Read-Host -Prompt "Press Enter to exit"
    }
}

for ($i = 0; $i -le 3; $i++) {
	if($i -ne 2)
	{
		Test-Level -level $i
	}
}