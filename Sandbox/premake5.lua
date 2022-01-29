-- Inlude paths for Sanbox
IncludeDir = {}
IncludeDir["GLFW"]          = "../Engine/vendor/glfw/include/"
IncludeDir["Glad"]          = "../Engine/vendor/glad/include/"
IncludeDir["ImGui"]         = "../Engine/vendor/imgui/"
IncludeDir["spdlog"]        = "../Engine/vendor/spdlog/include"
IncludeDir["cereal"]        = "../Engine/vendor/cereal/include"
IncludeDir["stb"]           = "../Engine/vendor/stb/"
IncludeDir["glm"]           = "../Engine/vendor/glm/"
IncludeDir["SPIRVReflect"]  = "../Engine/vendor/SPIRVReflect/"
IncludeDir["SPIRVCross"]    = "../Engine/vendor/SPIRVCross/include"
IncludeDir["entt"]    = "../Engine/vendor/entt/include"
IncludeDir["Razix"]         = "../Engine/src"
IncludeDir["vendor"]        = "../Engine/vendor/"

-- Sanbox Game project
project "Sandbox"
    kind "ConsoleApp"
    language "C++"

    -- Game source files
    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    -- Include paths
    includedirs
    {
        "../Engine/src/Razix",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}"
    }

    -- Macos include paths
    sysincludedirs
    {
        "../Engine/src/Razix",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}"
    }

   -- Razix Application linkage libraries
   links
   {
       "Razix", -- Razix DLL
       "glfw",
       "imgui",
       "spdlog",
       "SPIRVReflect",
       "SPIRVCross",
       "meshoptimizer",
       "OpenFBX"
   }

   defines
   {
       "SPDLOG_COMPILED_LIB"
   }

   -- Disable warning for vendor
   filter { "files:vendor/**"}
       warnings "Off"

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "latest"
        -- entrypoint "WinMainCRTStartup"

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_ROOT_DIR="  .. root_dir,
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_OPENGL",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX11",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Vidual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
        }

        links
        {
            "OpenGL32"
        }

        disablewarnings { 4307 }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION" }
        symbols "Off"
        optimize "Full"
