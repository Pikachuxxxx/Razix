#include "rzxpch.h"
#include "VKSwapchain.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        VKSwapchain::VKSwapchain(uint32_t width, uint32_t height)
        {
            m_Width = width;
            m_Height = height;

            // Create the swapchain
            Init();
        }

        VKSwapchain::~VKSwapchain()
        {

        }

        void VKSwapchain::Init()
        {
            // Query the swapchain surface properties
            querySwapSurfaceProperties();

            // Get the color space format for the swapchain images
            m_SurfaceFormat = chooseSurfaceFomat();

            // Choose the presentation mode
            m_PresentMode = choosePresentMode();

            // Choose the swapchain extent
            m_SwapchainExtent = chooseSwapExtent();

            // Create the KHR Swapchain
            createSwapchain();

            // Retrieve and create the swapchain images
            std::vector<VkImage> images = retrieveSwapchainImages();

            // Create image view for the retrieved swapchain images
            std::vector<VkImageView> imageView = createSwapImageViews(images);

            // Encapsulate the swapchain images and image views in a RZTExture2D
            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
                VKTexture2D* swapImageTexture = new VKTexture2D(images[i], imageView[i]);
                m_SwapchainImageTextures.push_back(swapImageTexture);
            }
        }

        void VKSwapchain::Destroy()
        {
            for (uint32_t i = 0; i < m_SwapchainImageCount; i++) {
               auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
               tex->Release(false);
            }
            vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
        }

        void VKSwapchain::Flip()
        {

        }

        void VKSwapchain::querySwapSurfaceProperties()
        {
            // Get the surface capabilities
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &m_SwapSurfaceProperties.capabilities);

            // Get the surface formats supported
            uint32_t formatsCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, nullptr);
            m_SwapSurfaceProperties.formats.resize(formatsCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, m_SwapSurfaceProperties.formats.data());

            // Get the available present modes
            uint32_t presentModesCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, nullptr);
            m_SwapSurfaceProperties.presentModes.resize(presentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, m_SwapSurfaceProperties.presentModes.data());
        }

        VkSurfaceFormatKHR VKSwapchain::chooseSurfaceFomat()
        {
            // Get the right color space
            // Get the right image format for the swapchain images to present mode
            for (const auto& format : m_SwapSurfaceProperties.formats) {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    return format;
            }
            return m_SwapSurfaceProperties.formats[0];
        }

        VkPresentModeKHR VKSwapchain::choosePresentMode()
        {
            // Choose the right kind of image presentation mode for the  swapchain images
            for (const auto& presentMode : m_SwapSurfaceProperties.presentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return presentMode;
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D VKSwapchain::chooseSwapExtent()
        {
            // choose the best Swapchain resolution to present the image onto
            if (m_SwapSurfaceProperties.capabilities.currentExtent.width != UINT32_MAX)
                return m_SwapSurfaceProperties.capabilities.currentExtent;
            else {
                auto& capabilities = m_SwapSurfaceProperties.capabilities;
                int width, height;
                glfwGetFramebufferSize((GLFWwindow*)VKContext::Get()->getWindow()->GetNativeWindow(), &width, &height);

                VkExtent2D actualExtent =
                {
                   static_cast<uint32_t>(width),
                   static_cast<uint32_t>(height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        void VKSwapchain::createSwapchain()
        {
            // Get the swapchain image count
            m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.minImageCount + 1; // For triple buffering
            // Bound checking the swapchain image count only for triple buffer aka 2 frames in flight
            if (m_SwapSurfaceProperties.capabilities.maxImageCount > 0 && m_SwapchainImageCount > m_SwapSurfaceProperties.capabilities.maxImageCount)
                m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.maxImageCount;
            RAZIX_CORE_TRACE("[Vulkan] Swap images count : {0}", m_SwapchainImageCount);

            // Now create the Swapchain
            VkSwapchainCreateInfoKHR swcCI = {};
            swcCI.sType     = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swcCI.surface   = VKContext::Get()->getSurface();
            swcCI.minImageCount = m_SwapchainImageCount;
            swcCI.imageFormat = m_SurfaceFormat.format;
            swcCI.imageColorSpace = m_SurfaceFormat.colorSpace;
            swcCI.imageExtent = m_SwapchainExtent;
            swcCI.imageArrayLayers = 1;
            swcCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            VKPhysicalDevice::QueueFamilyIndices indices = VKDevice::Get().getPhysicalDevice().get()->getQueueFamilyIndices();
            uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.Graphics), static_cast<uint32_t>(indices.Present) };

            if (indices.Graphics != indices.Present) {
                swcCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swcCI.queueFamilyIndexCount = 2;
                swcCI.pQueueFamilyIndices = queueFamilyIndices;
            }
            else {
                swcCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swcCI.queueFamilyIndexCount = 0; // Optional
                swcCI.pQueueFamilyIndices = nullptr; // Optional
            }

            swcCI.preTransform = m_SwapSurfaceProperties.capabilities.currentTransform;
            swcCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swcCI.clipped = VK_TRUE;
            swcCI.oldSwapchain = VK_NULL_HANDLE;

            // Now actually create the swapchainManager
            if (VK_CHECK_RESULT(vkCreateSwapchainKHR(VKDevice::Get().getDevice(), &swcCI, nullptr, &m_Swapchain)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot create swapchain!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Swapchain created successfully!");
        }

        std::vector<VkImage> VKSwapchain::retrieveSwapchainImages()
        {
            std::vector<VkImage> swapImages; 
            
            uint32_t swapImageCount = 0;
            vkGetSwapchainImagesKHR(VKDevice::Get().getDevice(), m_Swapchain, &swapImageCount, nullptr);
            RAZIX_CORE_ASSERT((swapImageCount == m_SwapchainImageCount), "[Vulkan] Swapimage count doesn't match!");

            swapImages.resize(m_SwapchainImageCount);
            if (VK_CHECK_RESULT(vkGetSwapchainImagesKHR(VKDevice::Get().getDevice(), m_Swapchain, &m_SwapchainImageCount, swapImages.data())))
                RAZIX_CORE_ERROR("[Vulkan] Cannot retrieve swapchain images!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Swapchain images {0} have been retrieved successfully!", swapImageCount);

            return swapImages;
        }

        std::vector<VkImageView> VKSwapchain::createSwapImageViews(std::vector<VkImage> swapImages)
        {
            std::vector<VkImageView> swapchainImageViews;

            swapchainImageViews.resize(m_SwapchainImageCount);
            for (size_t i = 0; i < m_SwapchainImageCount; i++) {
                // Create the image view with the required properties
                VkImageViewCreateInfo imvCI{};
                imvCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imvCI.image = swapImages[i];
                imvCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
                imvCI.format = m_SurfaceFormat.format;
                imvCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                imvCI.subresourceRange.baseMipLevel = 0;
                imvCI.subresourceRange.levelCount = 1;
                imvCI.subresourceRange.baseArrayLayer = 0;
                imvCI.subresourceRange.layerCount = 1;
                if (VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &imvCI, nullptr, &swapchainImageViews[i])))
                    RAZIX_CORE_ERROR("[Vulkan] Cannot create swap image view!");
                else RAZIX_CORE_TRACE("[Vulkan] Swap Image view (id= {0} ) succesfully created!", i);
            }
            return swapchainImageViews;
        }

        void VKSwapchain::presentSwapchain(VkCommandBuffer& commandBuffer)
        {
            UNIMPLEMENTED
        }

        void VKSwapchain::createSynchronizationPrimitives()
        {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_RenderingFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            m_ImagesInFlight.resize(m_SwapchainImageCount);



        }

    }
}
