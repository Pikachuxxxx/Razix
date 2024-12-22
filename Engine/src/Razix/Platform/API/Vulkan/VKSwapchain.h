#pragma once

#include "RZSTL/smart_pointers.h"

#include "Razix/Gfx/RHI/API/RZSwapchain.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKFence.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        // Forward declaration
        class VKFence;

        struct FrameSyncData_VK
        {
            VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
            VkSemaphore renderingDoneSemaphore  = VK_NULL_HANDLE;
            //VkFence                   renderFence             = VK_NULL_HANDLE;
            rzstl::UniqueRef<VKFence> renderFence = nullptr;
        };

        class VKSwapchain : public RZSwapchain
        {
        public:
            /* Bunch of properties that define the swapchain surface */
            struct SwapSurfaceProperties
            {
                VkSurfaceCapabilitiesKHR        capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR>   presentModes;
            };

        public:
            VKSwapchain(u32 width, u32 height);
            ~VKSwapchain() {}

            void  Init(u32 width, u32 height) override;
            void  Destroy() override;
            void  DestroyBackBufferImages() override;
            void  Flip() override;
            void  OnResize(u32 width, u32 height) override;
            void* GetAPIHandle() override { return &m_Swapchain; }

            /* Creates synchronization primitives such as semaphores and fence for queue submit and present sync, basically syncs triple buffering */
            void createSynchronizationPrimitives() {}
            void createFrameData();

            // Flip related functions
            u32  acquireNextImage(VkSemaphore signalSemaphore);
            void queueSubmit(CommandQueue& commandQueue, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores);
            void present(VkSemaphore waitSemaphore);
            /* One show submit and presentation function to reduce nested function calling overhead! */
            void submitGraphicsAndFlip(CommandQueue& commandQueue);
            void submitCompute();

            RZTextureHandle   GetImage(u32 index) override { return m_SwapchainImageTextures[index]; }
            RZTextureHandle   GetCurrentImage() override { return m_SwapchainImageTextures[m_AcquiredBackBufferImageIndex]; }
            sz                GetSwapchainImageCount() override { return m_SwapchainImageCount; }
            FrameSyncData_VK& getCurrentFrameSyncDataVK()
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                return m_FramesSyncData[m_CurrentFrameIndex];
            }
            inline const VkFormat& getColorFormat() const { return m_ColorFormat; }
            VkSwapchainKHR         getSwapchain() const { return m_Swapchain; }

        private:
            VkSwapchainKHR               m_Swapchain                                   = VK_NULL_HANDLE;
            VkSwapchainKHR               m_OldSwapChain                                = VK_NULL_HANDLE;
            SwapSurfaceProperties        m_SwapSurfaceProperties                       = {};
            VkSurfaceFormatKHR           m_SurfaceFormat                               = {};
            VkPresentModeKHR             m_PresentMode                                 = {};
            VkExtent2D                   m_SwapchainExtent                             = {};
            u32                          m_SwapchainImageCount                         = {};
            std::vector<RZTextureHandle> m_SwapchainImageTextures                      = {};
            VkFormat                     m_ColorFormat                                 = {};
            FrameSyncData_VK             m_FramesSyncData[RAZIX_MAX_SWAP_IMAGES_COUNT] = {};
            std::vector<VkImageView>     m_ImageViews                                  = {};

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
            void                     destroyFrameSyncPrimitives();
            void                     destroyFrameSycnBackBuffers();
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
