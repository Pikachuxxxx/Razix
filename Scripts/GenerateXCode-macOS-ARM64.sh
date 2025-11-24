#!/bin/bash

echo "Generating XCode Project files for macOS ARM64..."
echo "=========================================================="
echo " ____           _        _____             _            "
echo "|  _ \ __ _ ___(_)_  __ | ____|_ __   __ _(_)_ __   ___ "
echo "| |_) / _  |_  / \ \/ / |  _| |  _ \ / _  | |  _ \ / _ |"
echo "|  _   (_| |/ /| |>  <  | |___| | | | (_| | | | | |  __/"
echo "|_| \_\__,_/___|_/_/\_\ |_____|_| |_|\__, |_|_| |_|\___|"
echo "                                     |___/              "
echo "                      🚧  🚧  🚧                         "
echo " Premake Build System CLI for Razix Engine - v0.4.3.dev "
echo "       @ 2025 copyright Pikachuxxxx - Phani Srikar      "
echo
echo "Welcome to Razix-Premake-Build CLI tool..."
echo "Generates XCode project files for macOS ARM64"
echo "=========================================================="
echo "- CLI for generating Project files and building"
echo "- Cross platform tool for installation for dev/user"
echo "=========================================================="
echo
echo "Target: macOS ARM64"
echo "Tool: Xcode 4"
echo "🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧  🚧"
echo "=========================================================="

chmod +x ./Tools/Building/premake/premake5
./Tools/Building/premake/premake5 --arch=arm64 --os=macosx xcode4
echo "Project files generated successfully!"
read -p "Press any key to continue..."