cd ..
@echo off
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
echo ========================================================== 
echo Welcome to Razix-Premake-Build CLI tool...
echo cleaning projects and build artifacts...  
echo ========================================================== 
call Tools\Building\premake\premake5.exe clean
PAUSE
