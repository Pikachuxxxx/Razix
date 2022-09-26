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

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "off"

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        symbols "Off"
        optimize "Full"
