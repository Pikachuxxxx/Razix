-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
------------------------------------------------------------------------------
-- Game Framework project
project "GameFramework"
    kind "WindowedApp"
    language "C++"

    -- Game source files
    files
    {
        "**.h",
        "**.cpp"
    }

    -- Macos include paths
    externalincludedirs
    {
        "../Engine/src/Razix",
        "../Engine",
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
    }

    -- Razix Application linkage libraries
    -- vendors (Tf am I linking these)
    links
    {
       "Razix", -- Razix DLL
        -- because of the client log macros this needs to be linked again because we didn't export the spdlog symbols first time
       "glfw",
       "imgui",
       "spdlog",
       "SPIRVReflect",
       "SPIRVCross",
       "lua",
       "optick",
       "Jolt",
       --"tracy",
    }

    defines
    {
       --"SPDLOG_COMPILED_LIB"
    }

    filter { "files:**.lua or *.razixproject or **.rzscn"}
        flags { "ExcludeFromBuild"}


     -- Disable warning for vendor

    filter { "files:vendor/**"}
       warnings "Off"

    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        -- entrypoint "WinMainCRTStartup"

        -- Build options for Windows / Visual Studio (MSVC)
        -- https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170 
        buildoptions
        {
            "/MP", "/bigobj"
        }

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR=./",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Visual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        disablewarnings { 4307, 4267, 4275, 4554, 4996 }

    filter "system:macosx"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "14.0"
        xcodebuildsettings {
            ["GENERATE_INFOPLIST_FILE"] = "YES"
        }
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
            "TRACY_ENABLE"
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
