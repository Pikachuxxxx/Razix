IncludeDir = {}
IncludeDir["GLFW"] = "vendor/glfw/include/"
IncludeDir["Glad"] = "vendor/glad/include/"
IncludeDir["ImGui"] = "vendor/imgui/"
IncludeDir["spdlog"] = "vendor/spdlog/include"
IncludeDir["stb"] = "vendor/stb/"
IncludeDir["Razix"] = "src"
IncludeDir["vendor"] = "vendor/"
IncludeDir["Vendor"] = "vendor/"


project "Razix"
    kind "SharedLib"
    language "C++"
    -- editandcontinue "Off"

    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "src/**.inl"
    }

    removefiles
    {
        "src/Razix/Platform/**"
    }

    includedirs
    {
        "",
        "../",
        "src/",
        "src/Razix",
    }

    sysincludedirs
    {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.external}",
        "%{IncludeDir.External}"
    }

    links
    {
        "imgui",
        "spdlog"
    }

    defines
    {
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- "IMGUI_USER_CONFIG=\"src/Razix/ImGui/ImConfig.h\"",
        "SPDLOG_COMPILED_LIB"
}

-- Vendor Build files
--filter 'files:src/Razix/Utilities/ExternalBuild.cpp'
buildoptions
{
    "-W3"
}

    -- Add special SSE optimization for 32-bit
    -- filter 'architecture:x86_64'
    --     defines { "RAZIX_SSE"}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"
        disablewarnings { 4307 }
        characterset ("MBCS")

        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        defines
        {
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_RENDER_API_OPENGL",
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
            "RAZIX_IMGUI",
            "RAZIX_USE_GLFW_WINDOWS"
        }

        files
        {
            "src/Razix/Platform/Windows/*.h",
            "src/Razix/Platform/Windows/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            "src/Razix/Platform/OpenGL/*.h",
            "src/Razix/Platform/OpenGL/*.cpp",

            "vendor/glad/src/glad.c"
        }

        links
        {
            "glfw",
            "Dbghelp"
        }

        buildoptions
        {
            "/MP", "/bigobj"
        }

        filter 'files:vendor/**.cpp'
            flags  { 'NoPCH' }
        filter 'files:vendor/**.c'
            flags  { 'NoPCH' }

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
        runtime "Release"
