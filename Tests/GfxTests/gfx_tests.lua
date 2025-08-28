-- common include dirs
include 'Scripts/premake/common/common_include_dirs.lua'

function ApplyGfxTestSettings()
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    links
    {
        "Razix",
        "TestShaders"
    }

    includedirs
    {
        "%{wks.location}/../Engine",
        "%{wks.location}/../Engine/src",
        "%{wks.location}/../Engine/src/Razix",
        "%{wks.location}/../Engine/internal",
        "%{wks.location}/../Engine/internal/RazixMemory",
        "%{wks.location}/../Engine/internal/RZSTL",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.optick}",
        "%{IncludeDir.Jolt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.D3D12MA}",
        "%{IncludeDir.dxc}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Experimental Vendor
        "%{ExperimentalIncludeDir.Eigen}",
        -- googletest vendor
        "%{wks.location}/../Tests/",
        "%{wks.location}/../Tests/vendor/googletest/googletest",
        "%{wks.location}/../Tests/vendor/googletest/googletest/include"
    }
    
    externalincludedirs
    {
        "%{wks.location}/../Engine",
        "%{wks.location}/../Engine/src",
        "%{wks.location}/../Engine/src/Razix",
        "%{wks.location}/../Engine/internal",
        "%{wks.location}/../Engine/internal/RazixMemory",
        "%{wks.location}/../Engine/internal/RZSTL",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.optick}",
        "%{IncludeDir.Jolt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.D3D12MA}",
        "%{IncludeDir.dxc}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Experimental Vendor
        "%{ExperimentalIncludeDir.Eigen}",
        -- googletest vendor
        "%{wks.location}/../Tests/",
        "%{wks.location}/../Tests/vendor/googletest/googletest",
        "%{wks.location}/../Tests/vendor/googletest/googletest/include"
    }

    defines
    {
        "RAZIX_TEST",
        -- RHI
        "RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES",
    }

    links
    {
        "RHI",
        "Razix", -- Razix DLL
        "googletest"
    }

    filter "system:windows"
        editandcontinue "Off"

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_OPENGL",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX11",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Visual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        disablewarnings
        {
            "4996"
        }

        includedirs
        {
            VulkanSDK .. "/include",
            "%{wks.location}/../Engine/vendor/winpix/Include/WinPixEventRuntime"
        }

    filter "system:macosx"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "14.0"

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_MACOS",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_METAL",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
        }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
end

group "Tests/GfxTests"
    include "HelloWorldTests/hello_world_tests.lua"
    include "ComputeTest/compute_tests.lua"

    -- GFX TEST SHADERS, all gfx tests can use this project to build and manage HLSL/Razix shaders
    project "TestShaders"
        kind "Utility"

        files
        { 
            -- Shader files
            "../../../Engine/content/Shaders/ShaderCommon/**",
            -- HLSL - primary language for all platforms shader gen
            "HelloWorldTests/Shaders/HLSL/**.hlsl",
            "HelloWorldTests/Shaders/HLSL/**.hlsli",
            "HelloWorldTests/Shaders/HLSL/**.vert.hlsl",
            "HelloWorldTests/Shaders/HLSL/**.geom.hlsl",
            "HelloWorldTests/Shaders/HLSL/**.frag.hlsl",
            "HelloWorldTests/Shaders/HLSL/**.comp.hlsl",
            -- Razix Shader File
            "HelloWorldTests/Shaders/Razix/**.rzsf",
            -- Compute Test shaders
            "ComputeTest/Shaders/HLSL/**.hlsl",
            "ComputeTest/Shaders/HLSL/**.comp.hlsl",
            "ComputeTest/Shaders/Razix/**.rzsf",
        }
group ""
