project "SpirvCross"
    kind "StaticLib"
    language "C++"
    systemversion "latest"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "spirv.h",
        "spirv.hpp",
        "spirv_cfg.cpp",
        "spirv_cfg.hpp",
        "spirv_common.hpp",
        "spirv_cpp.cpp",
        "spirv_cpp.hpp",
        "spirv_cross.cpp",
        "spirv_cross.hpp",
        "spirv_cross_c.cpp",
        "spirv_cross_c.h",
        "spirv_cross_containers.hpp",
        "spirv_cross_error_handling.hpp",
        "spirv_cross_parsed_ir.cpp",
        "spirv_cross_parsed_ir.hpp",
        "spirv_cross_util.cpp",
        "spirv_cross_util.hpp",
        "spirv_glsl.cpp",
        "spirv_glsl.hpp",
        "spirv_hlsl.cpp",
        "spirv_hlsl.hpp",
        "spirv_msl.cpp",
        "spirv_msl.hpp",
        "spirv_parser.cpp",
        "spirv_parser.hpp",
        "spirv_reflect.cpp",
        "spirv_reflect.hpp"
    }

    -- Cinfig settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG" }
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
