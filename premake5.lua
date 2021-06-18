require 'Scripts/premake-defines'
require 'Scripts/premake-triggers'
require 'Scripts/premake-settings'

-- Current root directory where the global premake file is located
root_dir = os.getcwd()

-- Using the command line to get the selected architecture
Arch = ""

if _OPTIONS["arch"] then
	Arch = _OPTIONS["arch"]
else
	if _OPTIONS["os"] then
		_OPTIONS["arch"] = "arm"
		Arch = "arm"
	else
		_OPTIONS["arch"] = "x64"
		Arch = "x64"
	end
end

-- The Razix Engine Workspace
workspace ( settings.workspace_name )
    location "build"
    startproject "Sandbox"
        flags 'MultiProcessorCompile'

    -- Output directory path based on build config
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    -- Binaries Output directory
    targetdir ("bin/%{outputdir}/")
    -- Intermediate files Output directory
    objdir ("bin-int/%{outputdir}/obj/")

    if Arch == "arm" then
		architecture "ARM"
	elseif Arch == "x64" then
		architecture "x86_64"
	elseif Arch == "x86" then
		architecture "x86"
	end

    print("Generating Project files for Architecture = ", Arch)

    -- Various build configuration for the engine
    configurations
    {
        "Debug",
        "Release",
        "Distribution"
    }

    group "Dependencies"
        require("Razix/vendor/ImGui/premake5")
        require("Razix/vendor/spdlog/premake5")
        require("Razix/vendor/GLFW/premake5")
    filter {}
    group ""

    -- Build Script for Razix Engine
    include "Razix/premake5"
    -- Build script for premake
    -- include "Sandbox/premake5"
