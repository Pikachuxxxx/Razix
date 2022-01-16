#pragma once

#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZTexture.h"

#ifdef RAZIX_RENDER_API_VULKAN 

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandPool.h"
#include "Razix/Platform/API/Vulkan/VKFence.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        #define MAX_SWAPCHAIN_BUFFERS 3

        struct FrameData
        {
            VkSemaphore presentSemaphore = VK_NULL_HANDLE, renderSemaphore = VK_NULL_HANDLE;

            Ref<VKFence> renderFence;
            Ref<VKCommandPool> commandPool;
            Ref<VKCommandBuffer> mainCommandBuffer;
        };

        class VKSwapchain : public RZSwapchain
        {
        public:
            /* Bunch of properties that define the swapchain surface */
            struct SwapSurfaceProperties
            {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };


            uint32_t getCurrentImageIndex() override { return m_CurrentBuffer; }

        public:
            VKSwapchain(uint32_t width, uint32_t height);
            ~VKSwapchain();

            void Init(uint32_t width, uint32_t height) override;
            void Destroy() override;
            void Flip() override;
            void OnResize(uint32_t width, uint32_t height) override;

            // Flip related functions
            /* Creates synchronization primitives such as semaphores and fence for queue submit and present sync, basically syncs triple buffering */
            void createSynchronizationPrimitives() {}
            void createFrameData();
            void acquireNextImage();
            void queueSubmit();
            //void OnResize(uint32_t width, uint32_t height, bool forceResize = false);
            void begin();
            void end();
            void present();

            RZTexture* GetImage(uint32_t index) override { return static_cast<RZTexture*>(m_SwapchainImageTextures[index]); }
            RZTexture* GetCurrentImage() override { return static_cast<RZTexture*>(m_SwapchainImageTextures[m_AcquireImageIndex]); }
            size_t GetSwapchainImageCount() override  {return m_SwapchainImageCount;}
            RZCommandBuffer* getCurrentCommandBuffer() override { return getCurrentFrameData().mainCommandBuffer.get(); }
            FrameData& getCurrentFrameData()
            {
                RAZIX_ASSERT(m_CurrentBuffer < m_SwapchainImageCount, "[Vulkan] Incorrect swapchain buffer index");
                return m_Frames[m_CurrentBuffer];
            }
            inline const VkFormat& getColorFormat() const { return m_ColorFormat; }

        private:
             VkSwapchainKHR             m_Swapchain = VK_NULL_HANDLE;                       /* Vulkan handle for swapchain, since it's a part of WSI we need the extension provided by Khronos  */
             VkSwapchainKHR             m_OldSwapChain = VK_NULL_HANDLE;
             SwapSurfaceProperties      m_SwapSurfaceProperties;                            /* Swapchain surface properties                                                                     */
             VkSurfaceFormatKHR         m_SurfaceFormat;                                    /* Selected Swapchain image format and color space of the swapchain image                           */
             VkPresentModeKHR           m_PresentMode;                                      /* The presentation mode for the swapchain images                                                   */
             VkExtent2D                 m_SwapchainExtent;                                  /* The extent of the swapchain images                                                               */
             uint32_t                   m_SwapchainImageCount;                              /* Total number of swapchain images being used                                                      */
             std::vector<RZTexture2D*>  m_SwapchainImageTextures;                           /* Swapchain images stored as engine 2D texture                                                     */
             uint32_t                   m_AcquireImageIndex;                                /* Currently acquired image index of the swapchain that is being rendered to                        */
             VkFormat                   m_ColorFormat;                                      /* Color format of the screen                             
            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = VKContext::Get();                                                      */
            FrameData m_Frames[MAX_SWAPCHAIN_BUFFERS];
            uint32_t m_CurrentBuffer = 0;  /* Index of the current buffer being submitted for execution */
            bool m_IsResized = false;

        private:
            /* Queries the swapchain properties such as presentation modes supported, surface formats and capabilities */
            void querySwapSurfaceProperties();
            /* Choose the best swapchain image surface format and color space after querying the supported properties */
            VkSurfaceFormatKHR chooseSurfaceFomat();
            /* Choose the required present modes by checking the supported present modes */
            VkPresentModeKHR choosePresentMode();
            /* Gets the swapchain image extents */
            VkExtent2D chooseSwapExtent();
            /* Creates the swapchain */
            void createSwapchain();
            /* Retrieves the Swapchain images */
            std::vector<VkImage> retrieveSwapchainImages();
            /* creates the image views for the swapchain */
            std::vector<VkImageView> createSwapImageViews(std::vector<VkImage> swapImages);
        };
    }
}
#endif
