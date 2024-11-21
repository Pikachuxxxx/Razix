-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'
------------------------------------------------------------------------------
-- Shaders a separate project to build as cache
include 'razix_shaders_build.lua'
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
group "Engine"
-- Razix project
project "Razix"
    kind "SharedLib"
    language "C++"
        -- Debugging directory = where the main premake5.lua is located
    debugdir "%{wks.location}../"

    -- Razix Engine defines (Global)
    defines
    {
        --Razix
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- Renderer
        "RAZIX_RENDERER_RAZIX",
        "RAZIX_RAY_TRACE_RENDERER_RAZIX",
        "RAZIX_RAY_TRACE_RENDERER_OPTIX",
        "RAZIX_RAY_TRACE_RENDERER_EMBREE",
        -- vendor
        "OPTICK_MSVC"
    }

    -- Razix Engine source files (Global)
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "src/**.inl",
        "src/**.tpp"
        -- vendor
        --"vendor/tracy/TracyClient.cpp",
    }

    -- Lazily add the platform files based on OS config
    removefiles
    {
        "src/Razix/Platform/**"
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
        -- Vendor
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
        -- API related
        -- Vulkan
        "%{VulkanSDK}",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}",
        "%{InternalIncludeDir.RZSTL}",
        "%{InternalIncludeDir.EASTL}",
        "%{InternalIncludeDir.EABase}"
    }

    -- Razix engine external linkage libraries (Global)
    links
    {
        "glfw",
        "imgui",
        "spdlog", -- Being linked staically by RazixMemory (Only include this in debug and release build exempt this in Distribution build)
        "SPIRVReflect",
        "SPIRVCross",
        "meshoptimizer",
        "OpenFBX", 
        "lua",
        "optick",
        "tracy",
        "Jolt",
        -- Shaders
        "Shaders",
        -- Razix Internal Libraries 
        -- 1. Razix Memory
        "RazixMemory",
        "RZSTL"
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
    filter { "files:vendor/**"}
        warnings "Off"


    -- default config file copy just in case some one starts the exe from bi directory
    postbuildcommands 
    {
        '{COPY} "%{wks.location}../Engine/content/config/DefaultEngineConfig.ini" "%{cfg.targetdir}/Engine/content/config/"',
    }

    -------------------------------------
    -- Razix Project settings for Windows
    -------------------------------------
    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        disablewarnings { 4307, 4267, 4275, 4715, 4554 } -- Disabling the 4275 cause this will propagate into everything ig, also 4715 = not returinign values from all control paths is usually done deliberately hence fuck this warning
        characterset ("MBCS")
        editandcontinue "Off"
        
        pchheader "src/rzxpch.h"
        pchsource "src/rzxpch.cpp"

         -- Enable AVX, AVX2, Bit manipulation Instruction set (-mbmi)
         -- because GCC uses fused-multiply-add (fma) instruction by default, if it is available. Clang, on the contrary, doesn't use them by default, even if it is available, so we enable it explicityly
        -- Only works with GCC and Clang
        --buildoptions { "-mavx", "-mavx2", "-mbmi", "-march=haswell"}--, "-mavx512f -mavx512dq -mavx512bw -mavx512vbmi -mavx512vbmi2 -mavx512vl"}
        --buildoptions {"/-fsanitize=address"}

        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        -- Build options for Windows / Visual Studio (MSVC)
        -- https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170 
        buildoptions
        {
            "/MP", "/bigobj", "/Zi", 
            "/WX" -- Treats all compiler warnings as errors! https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170
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
            "TRACY_ENABLE",
            -- build options
            "_DISABLE_VECTOR_ANNOTATION",
            "_DISABLE_STRING_ANNOTATION",
            "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
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
            "vulkan-1",
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
            '{COPY} "%{wks.location}../Engine/content/Shaders/Tools/dxc/bin/x64/dxil.dll" "%{cfg.targetdir}"'
        }
        
    -------------------------------------
    -- Razix Project settings for MacOS
    -------------------------------------
    filter "system:macosx"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        pchheader "%{wks.location}/../Engine/src/rzxpch.h"
        pchsource "%{wks.location}/../Engine/src/rzxpch.cpp"


    -- Config settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

        links
        {
            "WinPixEventRuntime",
            "WinPixEventRuntime_UAP"
        }

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

        links
        {
            "WinPixEventRuntime",
            "WinPixEventRuntime_UAP"
        }

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "On"
        optimize "Full"
        runtime "Release"
group""
