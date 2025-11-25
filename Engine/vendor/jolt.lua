project "Jolt"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17" -- Exception as Jolt was initially authored using C++17
    staticruntime "off"

    includedirs
    {
         "Jolt/",
         "Jolt/Jolt"
    }
    
    externalincludedirs
    {
         "Jolt/",
         "Jolt/Jolt"
    }

    files
    {
        "Jolt/Jolt/**.h",
        "Jolt/Jolt/**.cpp"
    }

    filter "system:linux"
        pic "On"

    filter "system:windows"
        systemversion "latest"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
