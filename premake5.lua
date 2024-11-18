-- Configuration settings
include 'Scripts/premake/common/premake-config.lua'
-- Engine tool include directories
include 'Scripts/premake/common/tool_includes.lua'
-- System overrides to support PS4/PS5/PS3 and XBOX ONE/X/SERIES X and other hardware systems
include 'Scripts/premake/extensions/system-overides.lua'

-- Workspace Settings
settings = { }
settings.workspace_name     = 'Razix'
settings.bundle_identifier  = 'com.PhaniSrikar'

-- Output files directories signature for bin and int-bin
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Current root directory where the global premake file is located
-- TODO: Use a fixed installation directory in program files insted of an arbitrary thing/ infact use this to verify the proper installation directory
root_dir = os.getcwd()
-- Add this as the installation directory to the engine config file
print("Generating Engine Config File...")
config_file = io.open( root_dir .. "/Engine/content/config/razix_engine.config", "w+")
io.output(config_file)
io.write("installation_dir=" .. root_dir)
run_as_admin = "runas /user:administrator"
set_env = run_as_admin .. "SETX RAZIX_SDK" .. root_dir .. " /m"
set_env_tools = run_as_admin .. "SETX RAZIX_SDK" .. root_dir .. "/Tools /m"
-- Set some Razix SDK env variables
os.execute(set_env)
os.execute(set_env_tools)


-- QT SDK - 5.15.2
QTDIR = os.getenv("QTDIR")

if (QTDIR == nil or QTDIR == '') then
    print("QTDIR Enviroment variable is not found! Please check your development environment settings")
    os.exit()
else
    print("QTDIR found at : " .. QTDIR)
end

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

-- Razix Engine Global Built Settings
engine_global_config = {
    -- Razix is compiled with C++14 (we need constexpr)
    -- Using C++17 since Jolt, Entt, Sol needs it. Once we remove the dependencies of the 
    -- engine on Sol and Entt we will reert back to C++14, with the sole exception of Jolt
    cpp_dialect = "C++17"
}

-- Function to apply global settings to a project
function apply_engine_global_config()
    if engine_global_config.cpp_dialect then
        cppdialect(engine_global_config.cpp_dialect)
    end
end


-- The Razix Engine Workspace
workspace ( settings.workspace_name )
    location "build"
    startproject "Sandbox"
    flags 'MultiProcessorCompile' --(this won't work with clang)

    -- Use clang on windows
    --toolset "clang"

    -- Output directory path based on build config
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    -- Complete locatoin
    workspace_location = "%{wks.location}"
    -- Binaries Output directory
    targetdir ("bin/%{outputdir}/")
    -- Intermediate files Output directory
    objdir ("bin-int/%{outputdir}/obj/")
    -- Debugging directory = where the main premake5.lua is located
    debugdir "%{wks.location}../"
    --symbolspath "bin-int/%{outputdir}/pdb/"

    -- Setting the architecture for the workspace
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

    apply_engine_global_config()

    -- Build scripts for the Razix vendor dependencies
    group "Dependencies"
        include "Engine/vendor/cereal/cereal.lua"
        include "Engine/vendor/glfw/glfw.lua"
        include "Engine/vendor/imgui/imgui.lua"
        include "Engine/vendor/lua/lua.lua"
        include "Engine/vendor/optick/optick.lua"
        include "Engine/vendor/spdlog/spdlog.lua"
        include "Engine/vendor/SPIRVCross/SPIRVCross.lua"
        include "Engine/vendor/SPIRVReflect/SPIRVReflect.lua"
        include "Engine/vendor/tracy/tracy.lua"
        include "Engine/vendor/Jolt/jolt.lua"
    group ""

    -- Uses .NET 4.0
    framework "4.0"
    -- Sony WWS Authoring Tools Framework modules
    group "Dependencies/ATF"
        -- SCE ATF Dependencies
        --require("Tools/vendor/ATF/Framework/Atf.Core/premake5")
        --require("Tools/vendor/ATF/Framework/Atf.Gui/premake5")
        --require("Tools/vendor/ATF/Framework/Atf.Gui.WinForms/premake5")
        --require("Tools/vendor/ATF/Framework/Atf.IronPython/premake5")
        --require("Tools/vendor/ATF/Framework/Atf.SyntaxEditorControl/premake5")
    group ""

    -- Razix Tools Vendor dependencies
    group "Dependencies/Tools"
        include "Tools/RazixAssetPacker/vendor/assimp/assimp.lua"
        include "Tools/RazixAssetPacker/vendor/meshoptimizer/meshoptimizer.lua"
        include "Tools/RazixAssetPacker/vendor/OpenFBX/OpenFBX.lua"
    group ""

    -- Experimental
    group "Dependencies/Experimental"
        include "Engine/vendor/Experimental/eigen/eigen.lua"
    group ""

    -- Build Script for Razix Engine (Internal)
    --------------------------------------------------------------------------------
    group "Engine/internal"
        include "Engine/internal/RazixMemory/razixmemory.lua"
        include "Engine/internal/RZSTL/rzstl.lua"
    group ""

    -- Build Script for Razix Engine (Core)
    --------------------------------------------------------------------------------
    group "Engine"
        include "Engine/razix_engine.lua"
    group ""

    -- Build Script for Razix Editor
    --------------------------------------------------------------------------------
    group "Editor"
        include "Editor/razix_editor.lua"
    group ""

    -- in-house extension libraries for Razix Editor
    group "Editor/internal"
        include "Editor/internal/QtNodeGraph/QtNodeGraph_razix.lua"
    group ""

    -- Editor vendors
    group "Editor/vendor"
        include "Editor/vendor/QGoodWindow/QGoodWindow.lua"
        include "Editor/vendor/qspdlog/qspdlog.lua"
        include "Editor/vendor/QtADS/QtADS.lua"
        include "Editor/vendor/toolwindowmanager/toolwindowmanager.lua"
    group ""

    --------------------------------------------------------------------------------
    -- Build script for Razix Game Framework
    group "Game Framework"
        include "GameFramework/razix_game_framework.lua"
    group ""

    --------------------------------------------------------------------------------
    -- Build script for Sandbox
    group "Sandbox"
        include "Sandbox/sandbox.lua"
    group ""

    --------------------------------------------------------------------------------
    -- Engine related tools
    group "Tools"
        include "Tools/RazixAssetPacker/razix_tool_asset_packer.lua"
    group ""

    group "Tools/CLI"
        include "Tools/RazixAssetPacker/razix_tool_asset_packer_cli.lua"
    group""

    group "Tools/Build"
        -- premake scripts Utility project for in IDE management
        include "Tools/Building/premake/premake_regenerate_proj_files.lua"
        -- Gets the version of the Engine for Build workflows
        include "Tools/Building/RazixVersion/razix_tool_version.lua"
        -- Game Packager using Game Framework
        include "Tools/Building/GamePackager/game_packager.lua"
    group ""

    -- Razix Engine Samples and Tests
    --------------------------------------------------------------------------------
    -- TODO: Tests (recrusively projects are added)
    include "Tests/tests.lua"
    -- TODO: Samples
