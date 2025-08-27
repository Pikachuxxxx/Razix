project "Tracy"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files
    {
        --"client/**.h",
        --"client/**.hpp",
        --"client/**.c",
        --"client/**.cpp",
        "public/tracy/Tracy.hpp",
        --"TracyC.h",
        "public/TracyClient.cpp"
        --"TracyD3D11.hpp",
        --"TracyD3D12.hpp",
        --"TracyLua.hpp",
        --"TracyOpenCL.hpp",
        --"TracyOpenGL.hpp",
        --"TracyVulkan.hpp"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"

    -- Config settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "TRACY_ENABLE", "TRACY_ON_DEMAND", "_DEBUG"}
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG", "TRACY_ENABLE", "TRACY_ON_DEMAND", }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
