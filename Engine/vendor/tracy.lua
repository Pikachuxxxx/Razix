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
        "tracy/public/tracy/Tracy.hpp",
        --"TracyC.h",
        "tracy/public/TracyClient.cpp"
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

    filter "system:linux"
        buildoptions 
        { 
            "-fPIC" 
        }

     -- Tracy needs these defintions extra to work properly for implementation
    filter "configurations:Debug"
        defines {"TRACY_ENABLE", "TRACY_ON_DEMAND"}

