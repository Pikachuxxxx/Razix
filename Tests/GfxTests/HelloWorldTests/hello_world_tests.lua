-- common include dirs
include 'Scripts/premake/common/common_include_dirs.lua'

project "GfxTest-HelloTriangleTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "../../TestCommon/*.h",
        "../../TestCommon/*.cpp",
        "./Passes/RZHelloTriangleTestPass.h",
        "./Passes/RZHelloTriangleTestPass.cpp",
        "./HelloTriangleTest.cpp",
    }
    ApplyGfxTestSettings()
------------------------------------------------------------------------------
project "GfxTest-HelloTextureTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "../../TestCommon/*.h",
        "../../TestCommon/*.cpp",
        "./Passes/RZHelloTextureTestPass.h",
        "./Passes/RZHelloTextureTestPass.cpp",
        "./HelloTextureTest.cpp",
    }
    ApplyGfxTestSettings()
------------------------------------------------------------------------------
project "GfxTest-WaveIntrinsicsTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "../../TestCommon/*.h",
        "../../TestCommon/*.cpp",
        "./Passes/RZWaveIntrinsicsTestPass.h",
        "./Passes/RZWaveIntrinsicsTestPass.cpp",
        "./WaveIntrinsicsTest.cpp",
    }
    ApplyGfxTestSettings()