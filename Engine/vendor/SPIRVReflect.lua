project "SPIRVReflect"
    kind "StaticLib"
    language "C"

    files
    {
        "SPIRVReflect/common/output_stream.h",
        "SPIRVReflect/common/output_stream.cpp",
        "SPIRVReflect/include/spirv/unified1/spirv.h",
        "SPIRVReflect/spirv_reflect.c",
        "SPIRVReflect/spirv_reflect.h",
        "SPIRVReflect/spirv_reflect.cpp"

    }

    includedirs
    {
        "SPIRVReflect/include",
        "SPIRVReflect/./"
    }
    
    externalincludedirs
    {
        "SPIRVReflect/include",
        "SPIRVReflect/"
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

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
