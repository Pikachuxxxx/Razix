#pragma once

#include "RZSTL/smart_pointers.h"

#include "Razix/Core/OS/RZWindow.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKDevice.h"
    #include "Razix/Platform/API/Vulkan/VKSwapchain.h"

    #include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Razix {
    namespace Graphics {

        class RAZIX_API VKContext : public RZGraphicsContext
        {
        public:
            VKContext(RZWindow* windowHandle);

            void Init() override;
            void Destroy() override;
            void ClearWithColor(f32 r, f32 g, f32 b) override {}

            void Wait() override;

            // TODO: Remove this and use a method on the RZGraphicsContext side to retrieve this!
            /* Gets the underlying Vulkan context object */
            static VKContext* Get() { return static_cast<VKContext*>(s_Context); }

            /* creates the WSI surface to render the the stuff, required the platform window handle to create the surface */
            void CreateSurface(void* window);

            void SetupDeviceAndSC();

            rzstl::Ref<VKSwapchain>& getSwapchain() { return m_Swapchain; }

            /* Gets the reference to the Vulkan instance handle */
            inline const VkInstance& getInstance() const { return m_Instance; }
            /* Gets the vulkan KHR surface object handle */
            inline const VkSurfaceKHR& getSurface() const { return m_Surface; }
            /*  Returns a const pointer to the window handle that the context renders to */
            inline const RZWindow* getWindow() const { return m_Window; }
   

        private:
            RZWindow*                          m_Window;                         /* The Window handle                                    */
            VkInstance                         m_Instance;                       /* The Vulkan instance handle                           */
            bool                               m_EnabledValidationLayer = true;  /* Whether or not to enable validation layers on not    */
            std::vector<VkLayerProperties>     m_InstanceLayers;                 /* Collection of list of Instance layers supported      */
            std::vector<VkExtensionProperties> m_InstanceExtensions;             /* Collection of list of supported Extension            */
            std::vector<cstr>                  m_RequiredInstanceLayerNames;     /* The list of Required Layers by the engine            */
            std::vector<cstr>                  m_RequiredInstanceExtensionNames; /* The list of Required Extension by the engine         */
            VkDebugUtilsMessengerEXT           m_DebugCallbackHandle;            /* Debug callback handle to manage the Vulkan debug     */
            VkDebugUtilsMessengerCreateInfoEXT m_DebugCI;                        /* Debug callback handle creation struct                */
            VkSurfaceKHR                       m_Surface;                        /* The WSI Surface to which Vulkan presents to          */
            rzstl::Ref<VKSwapchain>            m_Swapchain;                      /* Handle to the Razix-Vulkan swapchain abstraction     */

        private:
            /* Creates a VkInstance to interface with the Vulkan library */
            void createInstance();
            /* gets the Required layers for the Vulkan application + GLFW requests */
            std::vector<cstr> getRequiredLayers();
            /* Gets the required extension that must be enabled at the instance level */
            std::vector<cstr> getRequiredExtensions();
            /* Sets the debug messenger this is used to record instance creation and deletion */
            void setupDebugMessenger();

            /* Vulkan debug callback reporting function */
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
        };

    }    // namespace Graphics
}    // namespace Razix
#endif