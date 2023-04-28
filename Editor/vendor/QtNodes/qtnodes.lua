-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'
-- QT support for premake and VS
include 'Scripts/premake/extensions/qt/qt.lua'

-- https://stackoverflow.com/questions/69678689/address-sanitizer-in-msvc-why-does-it-report-an-error-on-startup

local qt = premake.extensions.qt

project "QtNodes"
    kind "StaticLib"
    language "C++"
    staticruntime "off"

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
	qtmodules { "core", "gui", "widgets", "opengl" }

    --
	-- Setup the prefix of the Qt libraries. Usually it's Qt4 for Qt 4.x
	-- versions and Qt5 for Qt 5.x ones. Again, this apply to the current
	-- configuration. So if you want to have a configuration which uses
	-- Qt4 and one that uses Qt5, you can do it.
	--
	qtprefix "Qt5"

    qtbinpath "C:/Qt/Qt_msvc_dir/msvc2017_64/bin"

    qtgenerateddir "%{prj.location}/../Editor/vendor/QtNodes/generated"

    files
    {
        -- C++ source files
        "src/**.c",
        "src/**.cpp",
        "src/**.h",
        "src/**.hpp",
        --"generated/**.cpp",
        "include/**.c",
        "include/**.cpp",
        "include/**.h",
        "include/**.hpp",
        "include/**",
        -- resource files
        "**.ui",
        "**.qrc",
        "**.qss",
        "**.css",
        "**.rcc",
        "**.png",
        "**.ico",
        --"**.svg",
        -- TODO: Add this under windows only
        "**.rc"
    }

    removefiles { "src/generated/**" }

     -- Macos include paths
    externalincludedirs
    {
        "src/",
        "include/",
        "include/QtNodes/",
        "include/QtNodes/internal"
    }

    includedirs
    {
        "src/",
        "include/",
        "include/QtNodes/",
        "include/QtNodes/internal"
    }

    libdirs
    {
        "bin/%{outputdir}/"
    }

    filter "files:**.ui or **.qrc or **.png or **.ico or **.qss or **.css or **.rcc"
            removeflags "ExcludeFromBuild"
            buildaction "Embed"
    filter ""

    --filter "files:**.rc"
    --    buildmessage "converting %{file.relpath} to dds ..."
    --    buildaction "Resource"
    --filter ""

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
           -- Windows / Vidual Studio
           "WIN32_LEAN_AND_MEAN",
           "WIN32",
           "_WINDOWS",
           "_CRT_SECURE_NO_WARNINGS",
           "_DISABLE_EXTENDED_ALIGNED_STORAGE",
           "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
           "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
           -- QT/Proejct specific
           "QT_QTPROPERTYBROWSER_IMPORT",
           "NODE_EDITOR_SHARED",
           "NODE_EDITOR_EXPORTS",
           "QT_NO_KEYWORDS",
           "QT_CORE_LIB",
           "QT_WIDGETS_LIB",
           "QT_GUI_LIB",
           "QT_OPENGL_LIB"
       }
       -- https://www.qt.io/blog/2013/04/17/using-gccs-4-8-0-address-sanitizer-with-qt
       -- buildoptions {"/fsanitize=address", "/fno-omit-frame-pointer"}
       -- linkoptions {"/fsanitize=address"}

       disablewarnings { 4307 }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        qtsuffix "d"
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

