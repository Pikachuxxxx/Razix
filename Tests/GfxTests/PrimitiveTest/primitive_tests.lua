-- Primitive Test project
include 'Scripts/premake/common/common_include_dirs.lua'

project "GfxTest-PrimitiveTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "../../TestCommon/*.h",
        "../../TestCommon/*.cpp",
        "./Passes/RZPrimitiveTestPass.h",
        "./Passes/RZPrimitiveTestPass.cpp",
        "./PrimitiveTest.cpp",
    }
    ApplyGfxTestSettings()
