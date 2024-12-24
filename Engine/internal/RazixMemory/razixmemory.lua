project "RazixMemory"
    language "C++"
    kind "StaticLib"

    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "include/**.h",
        -- vendor
        "vendor/**.c",
        "vendor/**.cpp",
        "vendor/**.h"
    }

    externalincludedirs
    {
        "./include"
    }

     includedirs
    {
        "./include",
        "vendor"
    }

    removefiles
    {
        "vendor/mmgr/**"
    }

    filter "system:windows"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        
        defines
        {
            "RAZIX_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        disablewarnings { 4302, 4311} 

        
    filter "system:macosx"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
        systemversion "latest"
        
        defines
        {
            "RAZIX_PLATFORM_MACOSX",
            "RAZIX_PLATFORM_UNIX"
        }
        


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
