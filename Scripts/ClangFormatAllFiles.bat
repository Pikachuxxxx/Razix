@echo off
setlocal

REM Store the current directory
set "SCRIPT_DIR=%CD%"

REM Get the name of the current directory (e.g., "Scripts", "Engine", etc.)
for %%I in ("%SCRIPT_DIR%") do set "CURRDIR=%%~nxI"

REM Assume script lives in Scripts/
if /I "%CURRDIR%"=="Scripts" (
    cd ..
    set "PYTHON_SCRIPT=.\clang_format_razix.py"
) else (
    set "PYTHON_SCRIPT=Scripts\clang_format_razix.py"
)

REM Check if the Python script exists
if exist "%PYTHON_SCRIPT%" (
    python "%PYTHON_SCRIPT%"
) else (
    echo ❌ Error: Could not find %PYTHON_SCRIPT%
    pause
    exit /b 1
)

pause
endlocal

