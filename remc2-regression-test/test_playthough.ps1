$REMC2_EXECUTABLE = "$PSScriptRoot/../Release/remc2.exe"
$CONFIG_FILE_PATH = "$PSScriptRoot/playthough-config.json"
$DEMO_FILE_PATH = "$PSScriptRoot/Levels-1-4-Recording.bin"
    
Write-Output "Running playthough demo levels 1-4"
Write-Output $REMC2_EXECUTABLE
Write-Output $CONFIG_FILE_PATH
$process = Start-Process -Wait -PassThru -FilePath $REMC2_EXECUTABLE -ArgumentList "--play_file $DEMO_FILE_PATH --config_file_path $CONFIG_FILE_PATH"
$result = $process.ExitCode
	
if ($result -eq 0) {
    Write-Output "Playthough success!"
} else {
    Write-Output "Playthough failed!"
}


Read-Host -Prompt "Press ENTER or CTRL+C to exit" | Out-Null