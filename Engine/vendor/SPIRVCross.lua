project "SpirvCross"
    kind "StaticLib"
    language "C++"
    --systemversion "latest"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "SPIRVCross/spirv.h",
        "SPIRVCross/spirv.hpp",
        "SPIRVCross/spirv_cfg.cpp",
        "SPIRVCross/spirv_cfg.hpp",
        "SPIRVCross/spirv_common.hpp",
        "SPIRVCross/spirv_cpp.cpp",
        "SPIRVCross/spirv_cpp.hpp",
        "SPIRVCross/spirv_cross.cpp",
        "SPIRVCross/spirv_cross.hpp",
        "SPIRVCross/spirv_cross_c.cpp",
        "SPIRVCross/spirv_cross_c.h",
        "SPIRVCross/spirv_cross_containers.hpp",
        "SPIRVCross/spirv_cross_error_handling.hpp",
        "SPIRVCross/spirv_cross_parsed_ir.cpp",
        "SPIRVCross/spirv_cross_parsed_ir.hpp",
        "SPIRVCross/spirv_cross_util.cpp",
        "SPIRVCross/spirv_cross_util.hpp",
        "SPIRVCross/spirv_glsl.cpp",
        "SPIRVCross/spirv_glsl.hpp",
        "SPIRVCross/spirv_hlsl.cpp",
        "SPIRVCross/spirv_hlsl.hpp",
        "SPIRVCross/spirv_msl.cpp",
        "SPIRVCross/spirv_msl.hpp",
        "SPIRVCross/spirv_parser.cpp",
        "SPIRVCross/spirv_parser.hpp",
        "SPIRVCross/spirv_reflect.cpp",
        "SPIRVCross/spirv_reflect.hpp"
    }

    warnings "off"