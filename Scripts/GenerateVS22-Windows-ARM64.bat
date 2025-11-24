@echo off
cd ..
echo Generating Visual Studio 2022 Project files for Windows ARM64...
echo ========================================================== 
echo " ____           _        _____             _            "
echo "|  _ \ __ _ ___(_)_  __ | ____|_ __   __ _(_)_ __   ___ "
echo "| |_) / _  |_  / \ \/ / |  _| |  _ \ / _  | |  _ \ / _ |"
echo "|  _   (_| |/ /| |>  <  | |___| | | | (_| | | | | |  __/"
echo "|_| \_\__,_/___|_/_/\_\ |_____|_| |_|\__, |_|_| |_|\___|"
echo "                                     |___/              "
echo "                      🚧  🚧  🚧                       "
echo " Premake Build System CLI for Razix Engine - v0.4.3.dev "
echo "       @ 2025 copyright Pikachuxxxx - Phani Srikar      "
echo 
echo Welcome to Razix-Premake-Build CLI tool...
echo Generates Visual Studio 2022 project files for Windows ARM64
echo ========================================================== 
echo - CLI for generating Project files and building
echo - Cross platform tool for installation for dev/user
echo ==========================================================   
echo 
echo Target: Windows ARM64
echo Tool: Visual Studio 2022
echo 🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧 
echo ==========================================================   
call Tools\Building\premake\premake5.exe --arch=arm64 vs2022
echo Project files generated successfully!
PAUSE