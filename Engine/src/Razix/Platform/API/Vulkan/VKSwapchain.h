#pragma once

#include "Razix/Graphics/API/Swapchain.h"
#include "Razix/Graphics/API/Texture.h"

#ifdef RAZIX_RENDER_API_VULKAN 

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKSwapchain : public RZSwapchain
        {
        public:
            /* Bunch of properties that define the swapchain model */
            struct SwapSurfaceProperties
            {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };

        public:
            VKSwapchain(uint32_t width, uint32_t height);
            ~VKSwapchain();

            void Init() override;
            void Destroy() override;
            void Flip() override;

            RZTexture* GetImage(uint32_t index) override { return static_cast<RZTexture*>(m_SwapchainImageTextures[index]); }
            RZTexture* GetCurrentImage() override { return static_cast<RZTexture*>(m_SwapchainImageTextures[m_AcquireImageIndex]); }
            size_t GetSwapchainImageCount() override  {return m_SwapchainImageCount;}
            RZCommandBuffer* GetCurrentCommandBuffer() override  {return nullptr;}

        private:
             VkSwapchainKHR             m_Swapchain;                /* Vulkan handle for swapchain, since it's a part of WSI we need the extension provided by Khronos  */
             SwapSurfaceProperties      m_SwapSurfaceProperties;    /* Swapchain surface properties                                                                     */
             VkSurfaceFormatKHR         m_SurfaceFormat;            /* Selected Swapchain image format and color space of the swapchain image                           */
             VkPresentModeKHR           m_PresentMode;              /* The presentation mode for the swapchain images                                                   */
             VkExtent2D                 m_SwapchainExtent;          /* The extent of the swapchain images                                                               */
             uint32_t                   m_SwapchainImageCount;      /* Total number of swapchain images being used                                                      */
             std::vector<RZTexture2D*>  m_SwapchainImageTextures;   /* Swapchain images stored as engine 2D texture                                                     */
             uint32_t                   m_AcquireImageIndex;        /* Currently acquired image index of the swapchain that is being rendered to                        */

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
