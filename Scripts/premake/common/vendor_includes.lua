IncludeDir = {}
-- Engine Vendors
IncludeDir["cereal"]            = "%{wks.location}/../Engine/" .. "vendor/cereal/include"
IncludeDir["Glad"]              = "%{wks.location}/../Engine/" .. "vendor/glad/include/"
IncludeDir["GLFW"]              = "%{wks.location}/../Engine/" .. "vendor/glfw/include/"
IncludeDir["ImGui"]             = "%{wks.location}/../Engine/" .. "vendor/imgui/"
IncludeDir["spdlog"]            = "%{wks.location}/../Engine/" .. "vendor/spdlog/include"
IncludeDir["stb"]               = "%{wks.location}/../Engine/" .. "vendor/stb/"
IncludeDir["glm"]               = "%{wks.location}/../Engine/" .. "vendor/glm/"
IncludeDir["SPIRVReflect"]      = "%{wks.location}/../Engine/" .. "vendor/SPIRVReflect/"
IncludeDir["SPIRVCross"]        = "%{wks.location}/../Engine/" .. "vendor/SPIRVCross/include"
IncludeDir["entt"]              = "%{wks.location}/../Engine/" .. "vendor/entt/include"
IncludeDir["lua"]               = "%{wks.location}/../Engine/" .. "vendor/lua/src"
IncludeDir["tracy"]             = "%{wks.location}/../Engine/" .. "vendor/tracy/public"
IncludeDir["optick"]            = "%{wks.location}/../Engine/" .. "vendor/optick/src"
IncludeDir["simde"]             = "%{wks.location}/../Engine/" .. "vendor/simde"
IncludeDir["Jolt"]              = "%{wks.location}/../Engine/" .. "vendor/Jolt"
IncludeDir["json"]              = "%{wks.location}/../Engine/" .. "vendor/json/single_include"
IncludeDir["D3D12MA"]           = "%{wks.location}/../Engine/" .. "vendor/D3D12MemoryAllocator/include"
IncludeDir["dxc"]               = "%{wks.location}/../Engine/" .. "vendor/dxc/inc"
IncludeDir["volk"]               = "%{wks.location}/../Engine/" .. "vendor/volk"

-- Experimental Stuff
ExperimentalIncludeDir = {}
ExperimentalIncludeDir["Eigen"]               = "%{wks.location}/../Engine/" .. "vendor/Experimental/eigen/"


EditorVendorIncludeDir = {}
-- Editor Vendors
EditorVendorIncludeDir["QGoodWindow"]       = "%{wks.location}/../Editor/" .. "vendor/QGoodWindow/src"
EditorVendorIncludeDir["qspdlog"]           = "%{wks.location}/../Editor/" .. "vendor/qspdlog/include"
EditorVendorIncludeDir["QtADS"]             = "%{wks.location}/../Editor/" .. "vendor/QtADS"
EditorVendorIncludeDir["toolwindowmanager"] = "%{wks.location}/../Editor/" .. "vendor/toolwindowmanager/src"

EditorVendorIncludeDir["editorvendor"]      = "%{wks.location}/../Editor/" .. "vendor/"

-- Engine Source
IncludeDir["Razix"]             = "%{wks.location}/../Engine/" .. "src"
IncludeDir["vendor"]            = "%{wks.location}/../Engine/" .. "vendor/"

-- Vulkan SDK
VulkanSDK = os.getenv("VULKAN_SDK")

if (VulkanSDK == nil or VulkanSDK == '') then
    print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
    os.exit()
else
    print("Vulkan SDK found at : " .. VulkanSDK)
end

-- TODO: provide util functions to set vendor includes for all projects
function setEngineVendorIncludes()
    includedirs
    {
        "%{wks.location}/../Engine",
        "%{wks.location}/../Engine/src",
        "%{wks.location}/../Engine/src/Razix",
        "%{wks.location}/../Engine/internal",
        "%{wks.location}/../Engine/internal/RazixMemory",
        "%{wks.location}/../Engine/internal/RZSTL",
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
        "%{IncludeDir.lua}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.optick}",
        "%{IncludeDir.Jolt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.D3D12MA}",
        "%{IncludeDir.dxc}",
        "%{IncludeDir.volk}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- Experimental Vendor
        "%{ExperimentalIncludeDir.Eigen}"
    }
end