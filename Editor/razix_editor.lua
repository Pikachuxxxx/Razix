
-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'
-- Razix Tools includes
include 'Scripts/premake/common/tool_includes.lua'
-- QT support for premake and VS
include 'Scripts/premake/extensions/qt/qt.lua'

-- https://stackoverflow.com/questions/69678689/address-sanitizer-in-msvc-why-does-it-report-an-error-on-startup

local qt = premake.extensions.qt

project "RazixEditor"
    kind "ConsoleApp"
    language "C++"

    pchheader "rzepch.h"
    pchsource "src/rzepch.cpp"

    files
    {
        -- C++ source files
        "src/**.c",
        "src/**.cpp",
        "src/**.h",
        "src/**.hpp",
        "src/**.inl",
        -- resource files
        "src/**.ui",
        "src/**.qrc",
        "src/**.qss",
        "src/**.css",
        "src/**.rcc",
        "src/**.png",
        "src/**.ico",
        "Resources/**.ui",
        "Resources/**.qrc",
        "Resources/**.qss",
        "Resources/**.css",
        "Resources/**.rcc",
        "Resources/**.png",
        "Resources/**.ico",
        --"**.svg",
        -- TODO: Add this under windows only
        "src/**.rc",
        "Resources/**.rc"
    }

    removefiles { "src/generated/**", ""}

    includedirs
    {
        "../Engine/src/Razix",
        "../Engine",
        "../Editor/src",
        "../Editor/internal",
        "../Editor/internal/QtNodeGraph/src",
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
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- API related
        "%{VulkanSDK}/Include",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}",
        "%{InternalIncludeDir.RZSTL}",
        "%{InternalIncludeDir.EASTL}",
        "%{InternalIncludeDir.EABase}",
        -- Tools Include Dirs
        "%{ToolIncludeDir.RazixAssetPacker}",
        -- Editor vendor
        "%{EditorVendorIncludeDir.editorvendor}",
        "%{EditorVendorIncludeDir.QGoodWindow}",
        "%{EditorVendorIncludeDir.qspdlog}",
        "%{EditorVendorIncludeDir.QtADS}",
        "%{EditorVendorIncludeDir.toolwindowmanager}",   
    }

    links
    {
        -- ofc links to Razix Engine after all
        "Razix",
        -- because of the client log macros this needs to be linked again because we didn't export the spdlog symbols first time
        "glfw",
        "imgui",
        "ImGui.lib",
        "spdlog",
        "SPIRVReflect",
        "SPIRVCross",
        "meshoptimizer",
        "OpenFBX",
        "lua",
        "optick",
        "tracy",
        -- Internal
        "RazixMemory",
        "QtNodeGraph",
        -- Razix Tools
        "RazixAssetPacker",
        -- Editor Vendor
        "QGoodWindow",
        --"qspdlog", -- DIsabled due to C++20 requirement
        "QtADS",
        "toolwindowmanager"
    }

    libdirs
    {
        "bin/%{outputdir}/",
        "%{root_dir}/bin/%{outputdir}/"
    }

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
	qtpath "C:/Qt/5.15.2/msvc2019_64/"

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

    qtbinpath "C:/Qt/5.15.2/msvc2019_64/bin"

    qtgenerateddir "%{prj.location}/../Editor/src/generated"


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
       cppdialect (engine_global_config.cpp_dialect)
       staticruntime "off"
       systemversion "latest"
       -- entrypoint "WinMainCRTStartup"
       
       -- Windows specific defines
       defines
       {
           -- Engine
           "RAZIX_PLATFORM_WINDOWS",
           "RAZIX_USE_GLFW_WINDOWS",
           "RAZIX_IMGUI",
           -- Remove this as it's causing whitespace issues while using MOC to compile QT classes
           --"RAZIX_ROOT_DIR="..'\"' .. root_dir .. '\"',
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
           "RAZIX_EDITOR",
           "RAZIX_CONFIG=" .. outputdir,
           "QT_QTPROPERTYBROWSER_IMPORT",
           -- Editor Vendor
           "NODE_EDITOR_SHARED",
           -- Editor Extensions
           "QGOODWINDOW",
           "QT_VERSION_QT5"
       }
       -- https://www.qt.io/blog/2013/04/17/using-gccs-4-8-0-address-sanitizer-with-qt
       -- buildoptions {"/fsanitize=address", "/fno-omit-frame-pointer"}
       -- linkoptions {"/fsanitize=address"}

       disablewarnings { 4307, 4267, 4275, 4715, 4554, 4141, 4996 } -- Disabling the 4275 cause this will propagate into everything ig, also 4715 = not returinign values from all control paths is usually done deliberately hence fuck this warning

    -- TODO: Add post build command to copy QT DLLs and Plugins folder

    --postbuildcommands {
    --}
    
    filter "system:macosx"
        qtbinpath "/opt/homebrew/opt/qt@5/bin"
        qtpath "/opt/homebrew/opt/qt@5"

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

