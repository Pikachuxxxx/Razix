-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- common include dirs
include 'Scripts/premake/common/common_include_dirs.lua'
------------------------------------------------------------------------------
-- Engine Config files
group "Engine/content"
    project "Config"
        kind "Utility"

        files
        {
            -- ini files
            "content/config/**.ini"
        }

        filter { "files:**.ini"}
            flags { "ExcludeFromBuild" }
group"" 
------------------------------------------------------------------------------
-- Frame Graph resources data for editing in VS
group "Engine/content"
    project "FrameGraphs"
        kind "Utility"

        files
        {
            -- Graph files
            "content/FrameGraphs/Graphs/**.json",
            -- Pass files
            "content/FrameGraphs/Passes/**.json",
        }

        filter { "files:**.json"}
            flags { "ExcludeFromBuild" }
group"" 
------------------------------------------------------------------------------
group "Engine/content"
    project "Shaders"
        kind "Utility"

        files
        { 
            -- Shader common files
            "content/Shaders/ShaderCommon/**",
            -- HLSL - primary language for all platforms shader gen
            "content/Shaders/HLSL/**.h",
            "content/Shaders/HLSL/**.hlsl",
            "content/Shaders/HLSL/**.hlsli",
            "content/Shaders/HLSL/**.vert.hlsl",
            "content/Shaders/HLSL/**.geom.hlsl",
            "content/Shaders/HLSL/**.frag.hlsl",
            "content/Shaders/HLSL/**.comp.hlsl",
            -- Razix Shader File
            "content/Shaders/Razix/**.rzsf",
        }
group"" 
------------------------------------------------------------------------------
group "Engine"
-- Razix project
project "Razix"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    -- Razix Engine defines (Global
    defines
    {
        --Razix
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- RHI
        "RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES",
        -- vendor
        "OPTICK_MSVC",
        "VK_NO_PROTOTYPES",
    }

    -- Razix Engine source files (Global)
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        -- vendor
        --"vendor/tracy/TracyClient.cpp",
        -- imgui
        "vendor/imgui/backends/imgui_impl_glfw.cpp",
        "vendor/imgui/backends/imgui_impl_vulkan.cpp",
    }

    -- Lazily add the platform files based on OS config
	-- Also remove the core module, they are compiled as a library
    removefiles
    {
        --------------------------
        -- just until we finish off RHI
        "src/Razix/Gfx/LIMBO_STATE/**",
        --------------------------
        "src/Razix/Platform/**",
        "src/Razix/Gfx/RHI/**",
        "src/Razix/Core/Memory/vendor/mmgr/mmgr.cpp",
    }

    -- For MacOS
    externalincludedirs
    {
        -- Engine
        "./",
        "../",
        "internal/",
        "src/",
        "src/Razix",
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

    -- Razix engine external linkage libraries (Global)
    links
    {
        -- RHI
        "RHI",
        "glfw",
        "imgui",
        "spdlog", -- Being linked staically by RazixMemory (Only include this in debug and release build exempt this in GoldMaster build)
        "SPIRVReflect",
        "SPIRVCross",
        "lua",
        "optick",
        "tracy",
        "Jolt",
        -- Shaders
        "Shaders",
    }

    flags 
    { 
        "FatalWarnings" -- Treat all warnings as errors
    }

    -- Disable PCH for vendors
    filter 'files:vendor/**.cpp'
        flags  { 'NoPCH' }
    filter 'files:vendor/**.c'
        flags  { 'NoPCH' }

    -- Disable warning for vendor
    filter { "files:vendor/**" }
        warnings "Off"
    filter { "files:src/Razix/Core/Memory/vendor/**" }
        warnings "Off"

    filter { "files:src/Razix/Core/Memory/vendor/mmgr/mmgr.cpp" }
        warnings "Off"
        removeflags { "FatalWarnings" }
        disablewarnings {4311, 4302, 4477}

    -------------------------------------
    -- Razix Project settings for Windows
    -------------------------------------
    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        systemversion "latest"
        disablewarnings { 4307, 4267, 4275, 4715, 4554 } -- Disabling the 4275 cause this will propagate into everything ig, also 4715 = not returinign values from all control paths is usually done deliberately hence fuck this warning
        characterset ("MBCS")
        
        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        -- Build options for Windows / Visual Studio (MSVC)
        -- https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170 
        buildoptions
        {
            "/MP", "/bigobj", 
            -- AVX2
            "/arch:AVX2", 
            -- TODO: enable FMA and AVX512
            -- Treats all compiler warnings as errors! https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170
        }

        linkoptions
        {
            "/WX:NO" -- until I figure out incremental disabling in release mode
        }

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_IMGUI",
            -- API
            -- "RAZIX_RENDER_API_OPENGL", // OpenGL is deprecated in Razix
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Visual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
            "TRACY_ENABLE", "TRACY_ON_DEMAND",
            -- build options
            "_DISABLE_VECTOR_ANNOTATION",
            "_DISABLE_STRING_ANNOTATION",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
        }

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/Windows/*.h",
            "src/Razix/Platform/Windows/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            -- Platform supported Graphics API implementatioon

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            "src/Razix/Platform/API/DirectX12/*.h",
            "src/Razix/Platform/API/DirectX12/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
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

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
        links
        {
            "Dbghelp",
            -- Render API
            "d3d11",
            "d3d12",
            "dxgi",
            "dxguid",
            "D3DCompiler",
            "dxcompiler"
        }

        -- Copy the DLLs to bin dir
        postbuildcommands 
        {
            -- copy windows pix dlls
            '{COPY} "%{wks.location}../Engine/vendor/winpix/bin/x64/WinPixEventRuntime.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../Engine/vendor/winpix/bin/x64/WinPixEventRuntime_UAP.dll" "%{cfg.targetdir}"',
            -- Copy the DXC dlls 
            '{COPY} "%{wks.location}../Engine/content/Shaders/Tools/dxc/bin/x64/dxcompiler.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../Engine/content/Shaders/Tools/dxc/bin/x64/dxil.dll" "%{cfg.targetdir}"',
                -- we are using RAZIX_ROOT_DIR for now
            -- Copy the engine conten folder with subdirs: config, Fonts, FrameGraphs, Logos, Shaders/Compiled, Splash, Textures
            -- [Docs]: https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/robocopy
            -- /E: Copies subdirectories. This option automatically includes empty directories.
            --'robocopy /E /MIR "%{wks.location}../Engine/content/config" "%{cfg.targetdir}"/Engine/content/config',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Fonts" "%{cfg.targetdir}"/Engine/content/Fonts',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/FrameGraphs" "%{cfg.targetdir}"/Engine/content/FrameGraphs',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Logos" "%{cfg.targetdir}"/Engine/content/Logos',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Shaders/Compiled" "%{cfg.targetdir}"/Engine/content/Shaders/Compiled',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Shaders/Razix" "%{cfg.targetdir}"/Engine/content/Shaders/Razix',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Splash" "%{cfg.targetdir}"/Engine/content/Splash',
            --'robocopy /E /MIR "%{wks.location}../Engine/content/Textures" "%{cfg.targetdir}"/Engine/content/Textures'
        }
        
    -------------------------------------
    -- Razix Project settings for MacOS
    -------------------------------------
    filter "system:macosx"
        cppdialect "C++17"
        systemversion "14.0" 
        --pchheader "rzxpch.h"
        --pchsource "src/rzxpch.cpp"

        linkoptions { "-rpath @executable_path/libRazix.dylib" }

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_MACOS",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_METAL",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
        }

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/MacOS/*.h",
            "src/Razix/Platform/MacOS/*.cpp",
            
            "src/Razix/Platform/Unix/*.h",
            "src/Razix/Platform/Unix/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            "src/Razix/Platform/API/Metal/*.h",
            "src/Razix/Platform/API/Metal/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
        }
    
        removefiles
        {
            --"src/rzxpch.cpp"
        }
        
        -- Windows specific incldue directories
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

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
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
        
        -- Apple Clang compiler options
        buildoptions
        {
            "-Wno-error=switch-enum",
            "-Wno-switch", "-Wno-switch-enum"
        }
        
                
        -- TEMP TEMP TEMP TEMP TEMP
        postbuildcommands
        {
            -- copy vulkan DLL until we use volk
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.dylib" "%{cfg.targetdir}/libvulkan.dylib"',
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.1.dylib" "%{cfg.targetdir}/libvulkan.1.dylib"'
            -- we are using RAZIX_ROOT_DIR for now
            -- Copy the engine content folder with subdirectories: config, Fonts, FrameGraphs, Logos, Shaders/Compiled, Splash, Textures
            -- [Docs]: https://linux.die.net/man/1/rsync
            -- we need to create the directories manually unlike robocopy
            --'mkdir -p "%{cfg.targetdir}/Engine/content/config/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Fonts/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/FrameGraphs/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Logos/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Shaders/Compiled/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Shaders/Razix/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Splash/"',
            --'mkdir -p "%{cfg.targetdir}/Engine/content/Textures/"',
            -- -a: Archive mode, preserves symbolic links, permissions, timestamps, etc.
            -- --delete: Ensures the destination matches the source exactly (like /MIR in robocopy)
            --'rsync -a --delete "%{wks.location}/../Engine/content/config/" "%{cfg.targetdir}/Engine/content/config/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Fonts/" "%{cfg.targetdir}/Engine/content/Fonts/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/FrameGraphs/" "%{cfg.targetdir}/Engine/content/FrameGraphs/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Logos/" "%{cfg.targetdir}/Engine/content/Logos/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Shaders/Compiled/" "%{cfg.targetdir}/Engine/content/Shaders/Compiled/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Shaders/Razix/" "%{cfg.targetdir}/Engine/content/Shaders/Razix/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Splash/" "%{cfg.targetdir}/Engine/content/Splash/"',
            --'rsync -a --delete "%{wks.location}/../Engine/content/Textures/" "%{cfg.targetdir}/Engine/content/Textures/"'
        }
        
        filter "files:**.c"
            flags { "NoPCH" }
        filter "files:**.m"
            flags { "NoPCH" }
        filter "files:**.mm"
            flags { "NoPCH" }

    -------------------------------------
    -- Razix Project settings for iOS
    -------------------------------------
    filter "system:ios"
		targetextension ".app"
        
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_IOS",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_METAL",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
        }

    -------------------------------------
    -- Razix Project settings for Linux
    -------------------------------------
    filter "system:linux"
        cppdialect "C++17"

        defines
        {
            -- Engine
            "RAZIX_PLATFORM_LINUX",
            "RAZIX_PLATFORM_UNIX",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_VULKAN",
            "TRACY_ENABLE", "TRACY_ON_DEMAND"
        }

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/Linux/*.h",
            "src/Razix/Platform/Linux/*.cpp",
            
            "src/Razix/Platform/Unix/*.h",
            "src/Razix/Platform/Unix/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
        }
    
        -- Windows specific incldue directories
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
        
        -- Clang/GCC compiler options
        buildoptions
        {
            "-Wno-error=switch-enum",
            "-Wno-switch", "-Wno-switch-enum"
        }
        
        filter "files:**.c"
            flags { "NoPCH" }
        filter "files:**.m"
            flags { "NoPCH" }
        filter "files:**.mm"
            flags { "NoPCH" }


    -- Config settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"
        editandcontinue "Off"

        filter "system:windows"
            links
            {
                "WinPixEventRuntime",
                "WinPixEventRuntime_UAP"
            }
            linkoptions
            {
                "/INCREMENTAL",--"/NODEFAULTLIB:libcpmt.lib" ,"/NODEFAULTLIB:msvcprt.lib", "/NODEFAULTLIB:libcpmtd.lib", "/NODEFAULTLIB:msvcprtd.lib"
            }
        filter {}

   
    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        symbols "On"
        runtime "Release"
        optimize "Speed"
        editandcontinue "Off"
        
        filter "system:windows"
            links
            {
                "WinPixEventRuntime",
                "WinPixEventRuntime_UAP"
            }
        filter {}

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        runtime "Release"
        optimize "Full"
        editandcontinue "Off"
group""


