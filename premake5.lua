workspace "Razix"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- premake includes of Dependencies
group "Dependencies"
    include "Razix/vendor/GLFW"
    include "Razix/vendor/glad"
    include "Razix/vendor/ImGui"
group ""

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Razix/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Razix/vendor/glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Razix/vendor/ImGui"

project "Razix"
    location "Razix"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

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
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    defines
    {
        "RZX_BUILD_DLL",
        "GLFW_INCLUDE_NONE"
    }

    filter "system:windows"
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
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines "RZX_RELEASE"
            runtime "Release"
            optimize "On"

        filter "configurations:Dist"
            defines "RZX_DIST"
            runtime "Release"
            symbols "Off"
            optimize "Full"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"

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
        systemversion "latest"

        defines
        {
            "RZX_PLATFORM_WINDOWS"
        }

        filter "configurations:Debug"
            defines "RZX_DEBUG"
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines "RZX_RELEASE"
            runtime "Release"
            optimize "On"

        filter "configurations:Dist"
            defines "RZX_DIST"
            runtime "Release"
            symbols "Off"
            optimize "Full"
