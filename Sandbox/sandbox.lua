-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

-- Sanbox Game project
project "Sandbox"
    kind "ConsoleApp"
    language "C++"
        -- Debugging directory = where the main premake5.lua is located
    debugdir "%{wks.location}../"

    buildoptions
    {
        -- Remove STL 
        "-nostdlib"
    }

    -- Game source files
    files
    {
        "src/**.h",
        "src/**.cpp",
        -- Scripting files for test
        "Assets/Scripts/**.lua"
    }

    -- Macos include paths
    sysincludedirs
    {
        "../Engine/src/Razix",
        "../Engine",
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
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}",
        "%{InternalIncludeDir.RZSTL}",
        "%{InternalIncludeDir.EASTL}",
        "%{InternalIncludeDir.EABase}"
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
       "meshoptimizer",
       "OpenFBX",
       "lua",
       "optick",
       "Jolt",
       --"tracy",
       -- Internal
       "RazixMemory",
       "RZSTL"
   }

   defines
   {
       --"SPDLOG_COMPILED_LIB"
   }

   filter { "files:**.lua"}
        flags { "ExcludeFromBuild"}

    filter { "system:Prospero" }
        targetextension ".elf"

   -- Disable warning for vendor
   filter { "files:vendor/**"}
       warnings "Off"

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "latest"
        -- entrypoint "WinMainCRTStartup"

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
            "RAZIX_RENDER_API_OPENGL",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX11",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Vidual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        disablewarnings { 4307 }

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

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
