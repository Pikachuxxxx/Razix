-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'

project "optick"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)

    files
    {
        "optick/src/**.h",
        "optick/src/**.hpp",
        "optick/src/**.c",
        "optick/src/**.cpp",
        "optick/src/optick_gpu.d3d12.cpp",
        "optick/src/optick_gpu.vulkan.cpp"
    }

    defines {"OPTICK_EXPORT"}

    -- Needed when using clang-cl on windows as it will default to linux as soon as it sees clang is being used
    --defines "OPTICK_MSVC"

    filter "system:windows"
        systemversion "latest"

         -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }