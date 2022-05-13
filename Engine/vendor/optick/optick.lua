project "optick"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    -- Vulkan SDK
    VulkanSDK = os.getenv("VULKAN_SDK")

    if (VulkanSDK == nil or VulkanSDK == '') then
        print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
        os.exit()
    else
        print("Vulkan SDK found at : " .. VulkanSDK)
    end

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.c",
        "src/**.cpp",
        "src/optick_gpu.d3d12.cpp",
        "src/optick_gpu.vulkan.cpp"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "off"

         -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }

        buildoptions { "-Wno-narrowing" }

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
