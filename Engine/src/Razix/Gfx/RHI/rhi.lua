-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- common include dirs
include 'Scripts/premake/common/common_include_dirs.lua'
------------------------------------------------------------------------------
-- Sanbox Game project
project "RHI"
    kind "SharedLib"
    language "C"
    cdialect "C99"

    -- RHI source files
    files
    {
        "RHI.h",
        "RHI.c",
    }

    -- include paths
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
        "%{IncludeDir.volk}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Experimental Vendor
        "%{ExperimentalIncludeDir.Eigen}",
    }

    defines
    {
        "RAZIX_EXPORT_SYMBOLS",
        "VK_NO_PROTOTYPES"
    }

    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        -- entrypoint "WinMainCRTStartup"
        
        files 
        {
            "./Backend/vk_rhi.h",
            "./Backend/vk_rhi.c",
            "./Backend/dx12_rhi.h",
            "./Backend/dx12_rhi.c",
        } 

        -- Build options for Windows / Visual Studio (MSVC)
        -- https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170 
        buildoptions
        {
            "/MP", "/bigobj"
        }

        linkoptions
        {
            --"/NODEFAULTLIB:libcpmt.lib" ,"/NODEFAULTLIB:msvcprt.lib", "/NODEFAULTLIB:libcpmtd.lib", "/NODEFAULTLIB:msvcprtd.lib"
        }

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Visual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
        }

        -- Windows specific incldue directories
        includedirs
        {
            VulkanSDK .. "/include",
            "%{wks.location}/../Engine/vendor/winpix/Include/WinPixEventRuntime"
        }

        -- Windows specific library directories
        libdirs
        {
            VulkanSDK .. "/Lib",
            "%{wks.location}/../Engine/vendor/winpix/bin/x64",
            "%{wks.location}/../Engine/vendor/dxc/lib/x64",
        }

        linkoptions {
            "/IGNORE:4006"
        }

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
        links
        {
            -- Render API
            "d3d11",
            "d3d12",
            "dxgi",
            "dxguid",
            "D3DCompiler",
            "dxcompiler",
        }

        disablewarnings { 4307, 4267, 4275, 4554, 4996, 4006 }
        
    -------------------------------------
    -- RHI Project settings for MacOSX
    -------------------------------------
    filter "system:macosx"
        staticruntime "off"
        systemversion "14.0"

        files 
        {
            "./Backend/vk_rhi.h",
            "./Backend/vk_rhi.c",
        } 

        linkoptions { "-rpath @executable_path/libRazix.dylib" }

        runpathdirs
        {
            "%{cfg.buildtarget.bundlepath}/"
        }

        embed {
            "libvulkan.1.dylib"
        }

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_MACOS",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_METAL",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
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

        links
        {
            -- Render API
            "IOKit.framework",
            "CoreFoundation.framework",
            "CoreVideo.framework",
            "CoreGraphics.framework",
            "AppKit.framework",
            "SystemConfiguration.framework"
        }


        postbuildcommands 
        {
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.dylib"',
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.1.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.1.dylib"',
            --'{COPY}  "%{wks.location}/../bin/%{outputdir}/libRazix.dylib" "%{cfg.buildtarget.bundlepath}/libRazix.dylib"'
        }

    -------------------------------------
    -- RHI Project settings for Linux
    -------------------------------------
    filter "system:linux"
        staticruntime "off"

        files 
        {
            "./Backend/vk_rhi.h",
            "./Backend/vk_rhi.c",
        } 

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_LINUX",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
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