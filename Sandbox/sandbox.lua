-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'
-- common include dirs
include 'Scripts/premake/common/common_include_dirs.lua'
------------------------------------------------------------------------------
-- Sanbox Game project
project "Sandbox"
    kind "WindowedApp"
    language "C++"

    buildoptions
    {
        -- Remove STL 
        --"-nostdlib"
    }

    -- Game source files
    files
    {
        "src/**.h",
        "src/**.cpp",
        -- Assets files for editing in VS
        "Assets/Scripts/**.lua",
        "*.razixproject",
        "*Assets/Scenes/**.rzscn"
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
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        disablewarnings { 4307, 4267, 4275, 4554, 4996 }
        
    filter "system:macosx"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "latest"

        linkoptions { "-rpath @executable_path/libRazix.dylib" }

        runpathdirs
        {
            "%{cfg.buildtarget.bundlepath}/"
        }

        embed {
            "libRazix.dylib",
            "libvulkan.1.dylib"
        }

        xcodebuildresources { "IconAssets.xcassets", "libMoltenVK.dylib" }

		xcodebuildsettings
		{
			['CODE_SIGN_IDENTITY'] = 'Mac Developer',
            ['PRODUCT_BUNDLE_IDENTIFIER'] = settings.bundle_identifier,
			['INFOPLIST_FILE'] = '../Engine/src/Razix/Platform/MacOS/Info.plist',
			['ASSETCATALOG_COMPILER_APPICON_NAME'] = 'AppIcon',
			['CODE_SIGN_IDENTITY'] = ''
        }

        files 
        {
            "%{wks.location}/../Engine/src/Razix/Platform/MacOS/IconAssets.xcassets"
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

        postbuildcommands 
        {
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.dylib"',
            '{COPY}  "%{VulkanSDK}/lib/libvulkan.1.dylib" "%{cfg.buildtarget.bundlepath}/libvulkan.1.dylib"',
            '{COPY}  "%{wks.location}/../bin/%{outputdir}/libRazix.dylib" "%{cfg.buildtarget.bundlepath}/libRazix.dylib"'
            --'install_name_tool -add_rpath "@executable_path/libRazix.dylib" "%{cfg.buildtarget.bundlepath}/Sandbox"'
        }

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
            "TRACY_ENABLE"
        }

        linkoptions { "-rpath @executable_path/libRazix.dylib" }

        runpathdirs
        {
            "%{cfg.buildtarget.bundlepath}/"
        }

        embed {
            "libRazix.dylib",
            "libvulkan.1.dylib"
        }

        xcodebuildresources { "IconAssets.xcassets", "libMoltenVK.dylib" }

		xcodebuildsettings
		{
			['CODE_SIGN_IDENTITY'] = 'Mac Developer',
            ['PRODUCT_BUNDLE_IDENTIFIER'] = settings.bundle_identifier,
			['INFOPLIST_FILE'] = '../Engine/src/Razix/Platform/iOS/Info.plist',
			['ASSETCATALOG_COMPILER_APPICON_NAME'] = 'AppIcon',
			['CODE_SIGN_IDENTITY'] = ''
        }

        files 
        {
            "%{wks.location}/../Engine/src/Razix/Platform/iOS/IconAssets.xcassets"
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
