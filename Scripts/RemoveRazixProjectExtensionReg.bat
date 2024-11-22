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

:: Inform the user about the removal process
echo Removing .razixproject file association and related registry entries...

:: Delete the .razixproject file extension association
reg delete "HKEY_CLASSES_ROOT\.razixproject" /f

:: Delete the Razix.ProjectFile association
reg delete "HKEY_CLASSES_ROOT\Razix.ProjectFile" /f

:: Notify the user of completion
echo Registry entries removed successfully!
pause