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
            VkSwapchainKHR               m_Swapchain                                   = VK_NULL_HANDLE; /* Vulkan handle for swapchain, since it's a part of WSI we need the extension provided by Khronos   */
            VkSwapchainKHR               m_OldSwapChain                                = VK_NULL_HANDLE; /* Caching old swapchain, requires when we need to re-create swapchain                               */
            SwapSurfaceProperties        m_SwapSurfaceProperties                       = {};             /* Swapchain surface properties                                                                      */
            VkSurfaceFormatKHR           m_SurfaceFormat                               = {};             /* Selected Swapchain image format and color space of the swapchain image                            */
            VkPresentModeKHR             m_PresentMode                                 = {};             /* The presentation mode for the swapchain images                                                    */
            VkExtent2D                   m_SwapchainExtent                             = {};             /* The extent of the swapchain images                                                                */
            u32                          m_SwapchainImageCount                         = {};             /* Total number of swapchain images being used                                                       */
            std::vector<RZTextureHandle> m_SwapchainImageTextures                      = {};             /* Swapchain images stored as engine 2D texture                                                      */
            VkFormat                     m_ColorFormat                                 = {};             /* Color format of the screen                                                                        */
            FrameSyncData_VK             m_FramesSyncData[RAZIX_MAX_SWAP_IMAGES_COUNT] = {};             /* Frame sync primitives                                                                             */

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
    }    // namespace Graphics
}    // namespace Razix
#endif
