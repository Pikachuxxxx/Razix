-- Razix Include Directories
IncludeDir = {}
IncludeDir["cereal"]    = "vendor/cereal/include"
IncludeDir["Glad"]      = "vendor/glad/include/"
IncludeDir["GLFW"]      = "vendor/glfw/include/"
IncludeDir["ImGui"]     = "vendor/imgui/"
IncludeDir["spdlog"]    = "vendor/spdlog/include"
IncludeDir["stb"]       = "vendor/stb/"
IncludeDir["glm"]       = "vendor/glm/"
IncludeDir["Razix"]     = "src"
IncludeDir["vendor"]    = "vendor/"

-- Vulkan SDK
VulkanSDK = os.getenv("VULKAN_SDK")

if (VulkanSDK == nil or VulkanSDK == '') then
    print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
    os.exit()
else
    print("Vulkan SDK found at : " .. VulkanSDK)
end

-- Razix project
project "Razix"
    kind "SharedLib"
    language "C++"

    pchheader "src/rzxpch.h"
    pchsource "src/rzxpch.cpp"

    -- Razix Engine defines (Global)
    defines
    {
        --Razix
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- Renderer
        "RAZIX_RENDERER_RAZIX",
        "RAZIX_RENDERER_FALCOR",
        "RAZIX_RAY_TRACE_RENDERER_RAZIX",
        "RAZIX_RAY_TRACE_RENDERER_OPTIX",
        "RAZIX_RAY_TRACE_RENDERER_EMBREE"
        -- vendor
    }

    -- Razix Engine source files (Global)
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "src/**.inl"
    }

    -- Lazily add the platform files based on OS config
    removefiles
    {
        "src/Razix/Platform/**"
    }

    -- Include paths
    includedirs
    {
        -- Engine
        "./",
        "../",
        "src/",
        "src/Razix",
        -- Vendor
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- API related 
        "%{VulkanSDK}"
    }

    -- For MacOS
    sysincludedirs
    {
        -- Engine
        "./",
        "../",
        "src/",
        "src/Razix",
        -- Vendor
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- API related 
        "%{VulkanSDK}"
    }

    -- Razix engine external linkage libraries (Global)
    links
    {
        "glfw",
        "imgui",
        "spdlog"
    }

    -- Build options for Razix Engine DLL
    buildoptions
    {
        --"-W3"
    }

    -- Disable PCH for vendors
    filter 'files:vendor/**.cpp'
        flags  { 'NoPCH' }
    filter 'files:vendor/**.c'
        flags  { 'NoPCH' }

     -- Disable warning for vendor
    filter { "files:vendor/**"}
        warnings "Off"

    -- Razix Project settings for Windows
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"
        disablewarnings { 4307 }
        characterset ("MBCS")
        editandcontinue "Off"

        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
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

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/Windows/*.h",
            "src/Razix/Platform/Windows/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            -- Platform supported Graphics API implementatioon
            "src/Razix/Platform/API/OpenGL/*.h",
            "src/Razix/Platform/API/OpenGL/*.cpp",

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            "src/Razix/Platform/API/DirectX11/*.h",
            "src/Razix/Platform/API/DirectX11/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
        }

        -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }

        -- Windows specific library directories
        libdirs
        {
            VulkanSDK .. "/Lib"
        }

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
        links
        {
            "Dbghelp",
            -- Redner API
            "vulkan-1",
            "d3d11",
            "D3DCompiler"
        }

        -- Build options for Windows / Visual Studio (MSVC)
        buildoptions
        {
            "/MP", "/bigobj"
        }

    -- Cinfig settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
