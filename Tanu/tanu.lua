------------------------------------------------------------------------------
-- Tanu Game Project - RZTN 
------------------------------------------------------------------------------

-- Common includes
include "Scripts/premake/common/vendor_includes.lua"
include "Scripts/premake/common/common_include_dirs.lua"

project "Tanu"
    -- kind "WindowedApp" -- TODO: Enable this once XCode 16 installation and other stuff is fixed
    kind "ConsoleApp"
    language "C++"

    -- Game source files
    files {
        "src/**.h",
        "src/**.cpp",

        -- Asset files (excluded later)
        "Assets/Scripts/**.lua",
        "*.razixproject",
        "Assets/Scenes/**.rzscn"
    }

    ------------------------------------------------------------------------------
    -- Include directories
    ------------------------------------------------------------------------------
    externalincludedirs {
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
        "%{IncludeDir.volk}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        "%{ExperimentalIncludeDir.Eigen}",
    }

    ------------------------------------------------------------------------------
    -- Linked Libraries
    ------------------------------------------------------------------------------
    links {
        "RHI",
        "Razix",
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

    ------------------------------------------------------------------------------
    -- Defines
    ------------------------------------------------------------------------------
    defines {
        "RAZIX_GAME",
        "RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES",
    }

    ------------------------------------------------------------------------------
    -- Exclusions
    ------------------------------------------------------------------------------
    filter { "files:**.lua or files:**.razixproject or files:**.rzscn" }
        flags { "ExcludeFromBuild" }

    filter { "files:vendor/**" }
        warnings "Off"

    ------------------------------------------------------------------------------
    -- Platform Filters
    ------------------------------------------------------------------------------

    -- Windows
    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        buildoptions { "/MP", "/bigobj" }

        includedirs {
            VulkanSDK .. "/include",
            "%{wks.location}/../Engine/vendor/winpix/Include/WinPixEventRuntime"
        }

        defines {
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX12",
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        disablewarnings { 4307, 4267, 4275, 4554, 4996 }

    ------------------------------------------------------------------------------
    -- macOS
    ------------------------------------------------------------------------------
    filter "system:macosx"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "14.0"

        linkoptions { "-rpath @executable_path/libRazix.dylib" }

        runpathdirs { "%{cfg.buildtarget.bundlepath}/" }

        embed {
            "libRazix.dylib",
            "libvulkan.1.dylib"
        }

        xcodebuildresources { "IconAssets.xcassets", "libMoltenVK.dylib" }

        xcodebuildsettings {
            ["CODE_SIGN_IDENTITY"] = "Mac Developer",
            ["PRODUCT_BUNDLE_IDENTIFIER"] = settings.bundle_identifier,
            ["INFOPLIST_FILE"] = "../Engine/src/Razix/Platform/MacOS/Info.plist",
            ["ASSETCATALOG_COMPILER_APPICON_NAME"] = "AppIcon",
            ["CODE_SIGN_IDENTITY"] = ""
        }

        files {
            "%{wks.location}/../Engine/src/Razix/Platform/MacOS/IconAssets.xcassets"
        }
        
        includedirs
        {
            VulkanSDK .. "/include"
        }
        
        externalincludedirs
        {
            VulkanSDK .. "/include",
            "./",
            "../"
        }

        libdirs
        {
            VulkanSDK .. "/lib"
        }

        defines {
            "RAZIX_PLATFORM_MACOS",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_METAL",
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND"
        }

        postbuildcommands {
            '{COPY} "%{VulkanSDK}/lib/libvulkan.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.dylib"',
            '{COPY} "%{VulkanSDK}/lib/libvulkan.1.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.1.dylib"',
            --'{COPY} "%{wks.location}/../bin/%{outputdir}/libRazix.dylib" "%{cfg.buildtarget.bundlepath}/libRazix.dylib"'
        }

    ------------------------------------------------------------------------------
    -- Linux
    ------------------------------------------------------------------------------
    filter "system:linux"
        cppdialect "C++17"
        staticruntime "off"

        defines {
            "RAZIX_PLATFORM_LINUX",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            "RAZIX_RENDER_API_VULKAN",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
        }

    ------------------------------------------------------------------------------
    -- Configurations
    ------------------------------------------------------------------------------
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        symbols "On"
        optimize "Speed"
        runtime "Release"

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"

