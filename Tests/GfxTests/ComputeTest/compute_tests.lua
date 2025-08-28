-- Compute Test projects
include 'Scripts/premake/common/common_include_dirs.lua'

project "GfxTest-ComputeTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        "../../TestCommon/*.h",
        "../../TestCommon/*.cpp",
        "./Passes/RZMandleBrotPass.h",
        "./Passes/RZMandleBrotPass.cpp",
        "./Passes/RZBlitToSwapchainPass.h",
        "./Passes/RZBlitToSwapchainPass.cpp",
        "./ComputeTest.cpp",
    }
    ApplyGfxTestSettings()
