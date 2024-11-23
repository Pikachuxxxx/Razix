cd ..
@echo off
echo Generating Visual Studio 2022 Project files...
echo ========================================================== 
echo " ____           _        _____             _            "
echo "|  _ \ __ _ ___(_)_  __ | ____|_ __   __ _(_)_ __   ___ "
echo "| |_) / _  |_  / \ \/ / |  _| |  _ \ / _  | |  _ \ / _ |"
echo "|  _   (_| |/ /| |>  <  | |___| | | | (_| | | | | |  __/"
echo "|_| \_\__,_/___|_/_/\_\ |_____|_| |_|\__, |_|_| |_|\___|"
echo "                                     |___/              "
echo "                      🚧  🚧  🚧                       "
echo " Premake Build System CLI for Razix Engine - v0.4.3.dev "
echo "       @ 2024 copyright Pikachuxxxx - Phani Srikar      "
echo 
echo Welcome to Razix-Premake-Build CLI tool...
echo Generates XCode project files for both MacOS/iOS
echo ========================================================== 
echo - CLI for generating Project files and building
echo - Cross platform tool for installation for dev/user
echo ==========================================================   
echo 
echo Usage premake5 
echo Options:
echo  --version: Displays the tool version and exit
echo  --help: Displays commands available and their usage   
echo THE TOOL IS A WIP...still adding options...
echo 🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧 
echo ==========================================================   
call Tools\Building\premake\premake5.exe --arch=x64 vs2022
PAUSE
