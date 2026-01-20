project "LZ4"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "lz4/lib/**.c",
        "lz4/lib/**.h"
    }

    filter "system:linux"
        pic "On"
