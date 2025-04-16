 
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
root_dir = os.getcwd()

function generate_default_engine_config()
    -- Add this as the installation directory to the engine config file
    -- TODO: Use a fixed installation directory in program files insted of an arbitrary thing/ infact use this to verify the proper installation directory
    print("Generating Engine Config File...")
    local root_dir = os.getcwd()

    -- Path to the configuration file
    local config_path = root_dir .. "/Engine/content/config/DefaultEngineConfig.ini"

    -- Create the file if it doesn't exist
    local config_file = io.open(config_path, "r")
    local config_content = ""

    if config_file then
        -- File exists, read the current contents
        config_content = config_file:read("*a")
        config_file:close()
    else
        -- File doesn't exist, create a new one with default content
        config_content = [[
        [Rendering]
        EnableAPIValidation = true
        EnableMSAA = false
        EnableBindless = true
        PerfMode = 0                    ; None/Fidelity/Performance 
        GfxQuality = 2                  ; High/Medium/Low depending on GPU
        PreferredResolution = 1         
        TargetFPS = 120                 ; 60/120
        MaxShadowCascades = 4
        MSAASamples = 4
    ]]
    end

    -- Write the updated content back to the file
    config_file = io.open(config_path, "w+")
    io.output(config_file)
    io.write(config_content)
    io.close()

    print("Updated configuration file at: " .. config_path)
end

generate_default_engine_config()

-- Set some Razix SDK env variables on Windows only
if os.target() == "windows" then
    run_as_admin = "runas /user:administrator"
    set_env = run_as_admin .. "SETX RAZIX_SDK" .. root_dir .. " /m >nul 2>&1"
    set_env_tools = run_as_admin .. "SETX RAZIX_SDK" .. root_dir .. "/Tools /m >nul 2>&1"
    os.execute(set_env)
    os.execute(set_env_tools)
end

-- Using the command line to get the selected architecture
Arch = ""

if _OPTIONS["arch"] then

    Arch = _OPTIONS["arch"]
else
    _OPTIONS["arch"] = "x64"
    Arch = "x64"
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

    editandcontinue "Off"

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
    -- Debugging directory = where the executable is located
    debugdir ("bin/%{outputdir}/")

    -- Setting the architecture for the workspace
    if Arch == "arm" then
        architecture "ARM"
    elseif Arch == "x64" then
        architecture "x86_64"
    elseif Arch == "x86" then
        architecture "x86"
    elseif Arch == "arm64" then
        architecture "ARM64"
    end

    print("Generating Project files for Architecture = ", Arch)

    -- Various build configuration for the engine
    configurations
    {
        "Debug",
        "Release",
        "GoldMaster"
    }

    apply_engine_global_config()

    ------------------------------------------------------------------------------
    -- Shaders build rules (declared at start for tests and demos to pickup)
    include 'Engine/razix_shaders_build_rules.lua'

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
    -- TODO: We will be using blender so VS tool and CLI tools will be added here

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
    group "Tools/Build"
        -- premake scripts Utility project for in IDE management
        include "Tools/Building/premake/premake_regenerate_proj_files.lua"
        -- Gets the version of the Engine for Build workflows
        include "Tools/Building/RazixVersion/razix_tool_version.lua"
        -- Game Packager using Game Framework
        include "Tools/Building/GamePackager/game_packager.lua"
    group ""

    -- Razix Engine Tests
    --------------------------------------------------------------------------------
    -- Tests
    include "Tests/tests.lua"

