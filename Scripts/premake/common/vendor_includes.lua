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
IncludeDir["tracy"]             = "%{wks.location}/../Engine/" .. "vendor/tracy"
IncludeDir["optick"]            = "%{wks.location}/../Engine/" .. "vendor/optick/src"
IncludeDir["simde"]             = "%{wks.location}/../Engine/" .. "vendor/simde"
IncludeDir["Jolt"]              = "%{wks.location}/../Engine/" .. "vendor/Jolt"
IncludeDir["json"]              = "%{wks.location}/../Engine/" .. "vendor/json/single_include"
IncludeDir["D3D12MA"]           = "%{wks.location}/../Engine/" .. "vendor/D3D12MemoryAllocator/include"


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