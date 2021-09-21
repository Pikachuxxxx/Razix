IncludeDir = {}
IncludeDir["GLFW"] = "../Engine/vendor/glfw/include/"
IncludeDir["Glad"] = "../Engine/vendor/glad/include/"
IncludeDir["ImGui"] = "../Engine/vendor/imgui/"
IncludeDir["spdlog"] = "../Engine/vendor/spdlog/include"
IncludeDir["cereal"] = "../Engine/vendor/cereal/include"
IncludeDir["stb"] = "../Engine/vendor/stb/"
IncludeDir["Razix"] = "../Engine/src"
IncludeDir["vendor"] = "../Engine/vendor/"
IncludeDir["Vendor"] = "../Engine/vendor/"

project "Sandbox"
    kind "ConsoleApp"
    language "C++"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    sysincludedirs
    {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.external}",
        "%{IncludeDir.External}"
    }

    includedirs
    {
        "../Engine/src/Razix",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.Razix}"
    }

    links
    {
        "Razix",
        "imgui",
        "spdlog"
    }

    defines
    {
        "SPDLOG_COMPILED_LIB"
    }

    filter { "files:vendor/**"}
        warnings "Off"

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        -- entrypoint "WinMainCRTStartup"

        defines
        {
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_RENDER_API_OPENGL",
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
            "RAZIX_ROOT_DIR="  .. root_dir,
        }

        libdirs
        {
        }

        links
        {
            "glfw",
            "OpenGL32",
        }

        disablewarnings { 4307 }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE"}
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines "RAZIX_DISTRIBUTION"
        symbols "Off"
        optimize "Full"
