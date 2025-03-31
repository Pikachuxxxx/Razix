-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

project "RZSTL"
    language "C++"
    kind "StaticLib"

    files
    {
        "src/**.cpp",
        "include/**.h"
    }

    externalincludedirs
    {
        "./include",
        "%{InternalIncludeDir.RazixMemory}"
    }

     includedirs
    {
        "./include",
        "%{InternalIncludeDir.RazixMemory}"
    }

    links
    {
        "RazixMemory"
    }

    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER" }
        symbols "Off"
        optimize "Full"
