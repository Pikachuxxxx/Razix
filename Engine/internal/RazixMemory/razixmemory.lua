project "RazixMemory"
    language "C++"
    kind "StaticLib"

    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "include/**.h"
    }

    sysincludedirs
    {
        "./include"
    }

     includedirs
    {
        "./include"
    }

    filter "system:windows"
        cppdialect "C++17"
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

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION" }
        symbols "Off"
        optimize "Full"
