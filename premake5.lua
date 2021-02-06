workspace "Razix"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Razix/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Razix/vendor/glad/include"

-- premake includes of Dependencies
include "Razix/vendor/GLFW"
include "Razix/vendor/glad"

project "Razix"
    location "Razix"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "rzxpch.h"
    pchsource "Razix/src/rzxpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}"
    }

    links
    {
        "GLFW",
        "Glad",
        "opengl32.lib"
    }

    defines
    {
        "RZX_BUILD_DLL",
        "GLFW_INCLUDE_NONE"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RZX_PLATFORM_WINDOWS"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

        filter "configurations:Debug"
            defines 
            {
                "RZX_DEBUG",
                "RZX_ENABLE_ASSERTS"
            }
            buildoptions "/MDd"
            symbols "On"

        filter "configurations:Release"
            defines "RZX_RELEASE"
            buildoptions "/MD"
            optimize "On"

        filter "configurations:Dist"
            defines "RZX_DIST"
            buildoptions "/MD"
            symbols "Off"
            optimize "Full"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Razix/vendor/spdlog/include",
        "Razix/src"
    }

    links
    {
        "Razix"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RZX_PLATFORM_WINDOWS"
        }

        filter "configurations:Debug"
            defines "RZX_DEBUG"
            buildoptions "/MDd"
            symbols "On"

        filter "configurations:Release"
            defines "RZX_RELEASE"
            buildoptions "/MD"
            optimize "On"

        filter "configurations:Dist"
            defines "RZX_DIST"
            buildoptions "/MD"
            symbols "Off"
            optimize "Full"
