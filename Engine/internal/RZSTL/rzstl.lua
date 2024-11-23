-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

project "RZSTL"
    language "C++"
    kind "StaticLib"

    files
    {
        "src/**.cpp",
        "include/**.h",
        "./vendor/EABase/include/**.h",
        "./vendor/EASTL/include/**.h",
        "./vendor/EASTL/source/**.cpp"
    }

    externalincludedirs
    {
        "./include",
        "./vendor/EABase/include",
        "./vendor/EABase/include/Common",
        "./vendor/EASTL/include",
        "%{InternalIncludeDir.RazixMemory}"
    }

     includedirs
    {
        "./include",
        "./vendor/EABase/include",
        "./vendor/EABase/include/Common",
        "./vendor/EASTL/include",
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

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION" }
        symbols "Off"
        optimize "Full"
