#pragma once

#include "Razix/Core/OS/Window.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN 

#include "Razix/Core/SmartPointers.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Razix {
    namespace Graphics {

        class VKContext : public RZGraphicsContext
        {
        public:
            VKContext(RZWindow* windowHandle);

            virtual void Init() override;
            virtual void Destroy() override;
            virtual void SwapBuffers() override {}
            void ClearWithColor(float r, float g, float b) override {}

            static VKContext* Get() { return static_cast<VKContext*>(s_Context); }
            VkInstance GetInstance() { return m_Instance; }
            VkSurfaceKHR GetSurface() const { return m_Surface; }

        private:
            RZWindow*                             m_Window;                           /* The Window handle                                    */
            VkInstance                          m_Instance;                         /* The Vulkan instance handle                           */
            bool                                m_EnabledValidationLayer;           /* Whether or not to enable validation layers on not    */
            std::vector<VkLayerProperties>      m_InstanceLayers;                   /* Collection of list of Instance layers supported      */
            std::vector<VkExtensionProperties>  m_InstanceExtensions;               /* Collection of list of supported Extension            */
            std::vector<const char*>            m_RequiredInstanceLayerNames;       /* The list of Required Layers by the engine            */
            std::vector<const char*>            m_RequiredInstanceExtensionNames;   /* The list of Required Extension by the engine         */
            VkDebugUtilsMessengerEXT            m_DebugCallbackHandle;              /* Debug callback handle to manage the Vulkan debug     */
            VkDebugUtilsMessengerCreateInfoEXT  m_DebugCI;                          /* Debug callback handle creation struct                */
            VkSurfaceKHR                        m_Surface;                          /* The WSI Surface to which Vulkan presents to          */
            UniqueRef<VKDevice>                 m_Device;                           /* The handle to the Vulkan Device and Physical device  */

        private:
            /* Creates a VkInstance to interface with the Vulkan library */
            void CreateInstance();
            std::vector<const char*> GetRequiredLayers();
            std::vector<const char*> GetRequiredExtensions();
            void SetupDebugMessenger();
            void CreateSurface(GLFWwindow* window);
            /* Vulkan debug callback reporting function */
            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
        };

    }
}
#endif