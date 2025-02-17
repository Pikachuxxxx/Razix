project "SPIRVReflect"
    kind "StaticLib"
    language "C"

    files
    {
        "common/output_stream.h",
        "common/output_stream.cpp",
        "include/spirv/unified1/spirv.h",
        "spirv_reflect.c",
        "spirv_reflect.h",
        "spirv_reflect.cpp"

    }

    includedirs
    {
        "include",
        "./"
    }
    
    externalincludedirs
    {
        "include",
        "./"
    }

    warnings "off"

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "off"

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG", "_DISABLE_VECTOR_ANNOTATION " }
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
