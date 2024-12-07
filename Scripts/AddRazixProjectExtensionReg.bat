@echo off

:: Check for Admin privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo This script requires administrative privileges.
    echo Requesting Administrator access...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

:: Place your commands here
echo Running as Administrator...

:: Get the current directory
set "CWD=%~dp0"

:: Replace backslashes with double backslashes for Registry compatibility
set "ICON_PATH=%CWD%..\Engine\content\Logos\RazixLogo64.ico"
set "SANDBOX_PATH=%CWD%..\bin\Debug-windows-x86_64\Sandbox.exe"

:: Add the .razixproject file association
reg add "HKEY_CLASSES_ROOT\.razixproject" /ve /d "Razix.ProjectFile" /f

:: Add the Razix.ProjectFile description
reg add "HKEY_CLASSES_ROOT\Razix.ProjectFile" /ve /d "Razix Engine Project File" /f

:: Add the default icon
reg add "HKEY_CLASSES_ROOT\Razix.ProjectFile\DefaultIcon" /ve /d "\"%ICON_PATH%\"" /f

:: Add the shell open command
reg add "HKEY_CLASSES_ROOT\Razix.ProjectFile\shell\open" /ve /d "Open" /f
reg add "HKEY_CLASSES_ROOT\Razix.ProjectFile\shell\open" /v Icon /d "\"%ICON_PATH%\"" /f
reg add "HKEY_CLASSES_ROOT\Razix.ProjectFile\shell\open\command" /ve /d "\"%SANDBOX_PATH%\" -f \"%%1\"" /f

echo Registry keys added successfully!
PAUSE