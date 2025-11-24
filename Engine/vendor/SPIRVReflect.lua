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