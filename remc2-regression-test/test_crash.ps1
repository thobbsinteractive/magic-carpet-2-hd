$REMC2_EXECUTABLE = "$PSScriptRoot/../Release/remc2.exe"
$CONFIG_FILE_PATH = "$PSScriptRoot/playthough-config.json"
$RECORDING_FILE_PATH = "$PSScriptRoot/Level-1-Crash-Recording.bin"

Write-Output "Running Demo"
Write-Output $REMC2_EXECUTABLE
Write-Output $CONFIG_FILE_PATH
$process = Start-Process -Wait -PassThru -FilePath $REMC2_EXECUTABLE -ArgumentList "--play_file $RECORDING_FILE_PATH --config_file_path $CONFIG_FILE_PATH"
$result = $process.ExitCode

if ($result -eq 0) {
    Write-Output "Demo completed ok"
} else {
    Write-Output "Demo failed"
}


Read-Host -Prompt "Press ENTER or CTRL+C to exit" | Out-Null