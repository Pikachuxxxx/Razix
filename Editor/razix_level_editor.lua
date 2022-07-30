-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'
-- QT support for premake and VS
include 'Scripts/premake/extensions/qt/qt.lua'

local qt = premake.extensions.qt

project "RazixLevelEditor"
    kind "ConsoleApp"
    language "C++"

    -- be carefull, this function enables Qt only for the current configuration.
    -- So if you want to enable it on all configuration, be sure that no filter
    -- is active when calling this (or reset the filter using `filter {}`
    qt.enable()

    -- TODO: use environment variables to set this
	-- Setup the Qt path. This apply to the current configuration, so
	-- if you handle x32 and x64, you can specify a different path
	-- for both configurations.
	--
	-- Note that if this is ommited, the addon will try to look for the
	-- QTDIR environment variable. If it's not found, then the script
	-- will return an error since it won't be able to find the path
	-- to your Qt installation.
	--
	qtpath "C:/Qt/Qt_msvc_dir/msvc2017_64"

    --
	-- Setup which Qt modules will be used. This also apply to the
	-- current configuration, so can you choose to deactivate a module
	-- for a specific configuration.
	--
	qtmodules { "core", "gui", "widgets" }

    --
	-- Setup the prefix of the Qt libraries. Usually it's Qt4 for Qt 4.x
	-- versions and Qt5 for Qt 5.x ones. Again, this apply to the current
	-- configuration. So if you want to have a configuration which uses
	-- Qt4 and one that uses Qt5, you can do it.
	--
	qtprefix "Qt5"

    files
    {
        -- C++ source files
        "src/*.cpp",
        "src/*.h",
        -- resource files
        "**.ui",
        "**.qrc",
        "**.png",
        "**.ico"
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
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}"
    }

    links
    {
        -- ofc links to Razix Engine after all
        "Razix"
    }

    libdirs
    {
        "bin/%{outputdir}/"
    }

    filter "files:**.ui or **.qrc or **.png or **.ico"
            buildaction "Embed"
    filter ""

    -- Disable warning for vendor
   filter { "files:vendor/**"}
       warnings "Off"

       filter "system:windows"
       cppdialect "C++17"
       staticruntime "off"
       systemversion "latest"
       -- entrypoint "WinMainCRTStartup"
       
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
           "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
           -- Editor 
           "RAZIX_EDITOR"
       }
       
       disablewarnings { 4307 }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        qtsuffix "d"
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION" }
        symbols "Off"
        optimize "Full"

