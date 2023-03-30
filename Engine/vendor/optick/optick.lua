-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'

project "optick"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.c",
        "src/**.cpp",
        "src/optick_gpu.d3d12.cpp",
        "src/optick_gpu.vulkan.cpp"
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

        --buildoptions { "-Wno-narrowing" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Distribution"
        runtime "Release"
        symbols "Off"
        optimize "Full"
