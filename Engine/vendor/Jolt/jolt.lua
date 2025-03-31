project "Jolt"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17" -- Exception as Jolt was initially authored using C++17
    staticruntime "off"

    includedirs
    {
         "./",
         "./Jolt"
    }
    
    externalincludedirs
    {
         "./",
         "./Jolt"
    }

    files
    {
        "Jolt/**.h",
        "Jolt/**.cpp"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"

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
