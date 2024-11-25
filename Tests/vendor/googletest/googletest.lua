-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

project "googletest"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    includedirs
    {
        "./",
        "./googletest",
        "./googletest/include/gtest",
        "./googletest/include",
        "%{common_include_dirs}"
    }
    
    -- macos/xcode
    externalincludedirs
    {
        "./",
        "./googletest",
        "./googletest/include/gtest",
        "./googletest/include",
        "%{common_include_dirs}"
    }

    files
    {
        "./googletest/src/**.h",
        "./googletest/src/**.cc",
        "./googletest/include/**.h"
    }
       
    defines 
    {
        "RAZIX_TEST"
    }

    disablewarnings 
    {
        "4006" -- LINKER: already linked in
    }


    filter "system:windows"
        systemversion "latest"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"

        linkoptions
        {
            "/IGNORE:4006" -- LINKER: already linked in 
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

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
