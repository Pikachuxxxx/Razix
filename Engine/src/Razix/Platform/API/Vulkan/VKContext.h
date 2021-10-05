#pragma once

#include "Razix/Core/OS/Window.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN 

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Razix {
    namespace Graphics {

        class VKContext : public GraphicsContext
        {
        public:
            VKContext(Window* windowHandle);

            virtual void Init() override;
            virtual void Destroy() override;
            virtual void SwapBuffers() override {}
            void ClearWithColor(float r, float g, float b) override;

            VkInstance GetInstance() const { return m_Instance; }

        private:
            Window*                             m_Window;                   /* The Window handle                                  */
            VkInstance                          m_Instance;                 /* The Vulkan instance handle                         */
            bool                                m_EnabledValidationLayer;   /* Whether or not to enable validation layers on not  */
            std::vector<VkLayerProperties>      m_InstanceLayers;
            std::vector<VkExtensionProperties>  m_InstanceExtensions;
            std::vector<const char*>            m_RequiredInstanceLayerNames;       /* The list of Required Layers */
            std::vector<const char*>            m_RequiredInstanceExtensionNames;   /* The list of Required Extension */
            VkDebugUtilsMessengerEXT            m_DebugCallbackHandle;
            VkDebugUtilsMessengerCreateInfoEXT  m_DebugCI;
            VkSurfaceKHR                        m_Surface;
        private:
            /* Creates a VkInstance to interface with the Vulkan library */
            void CreateInstance();
            std::vector<const char*> GetRequiredLayers();
            std::vector<const char*> GetRequiredExtensions();
            void SetupDebugMessenger();
            void CreateSurface(GLFWwindow* window);

            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                                VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                                void*                                       user_data);
        };

    }
}
#endif