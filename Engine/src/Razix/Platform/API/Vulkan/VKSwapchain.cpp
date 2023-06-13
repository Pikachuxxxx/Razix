// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKSwapchain.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandPool.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKFence.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        static void GetQueueCheckpointDataNV(VkQueue queue, u32* pCheckpointDataCount, VkCheckpointDataNV* pCheckPointData)
        {
            auto func = (PFN_vkGetQueueCheckpointDataNV) vkGetDeviceProcAddr(VKDevice::Get().getDevice(), "vkGetQueueCheckpointDataNV");
            if (func != nullptr)
                func(queue, pCheckpointDataCount, pCheckPointData);
        }

        VKSwapchain::VKSwapchain(u32 width, u32 height)
        {
            m_Width  = width;
            m_Height = height;

            // Initialize the swapchain
            Init(m_Width, m_Height);
        }

        VKSwapchain::~VKSwapchain()
        {
        }

        void VKSwapchain::Init(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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

            // Encapsulate the swapchain images and image views in a RZTexture2D
            m_SwapchainImageTextures.clear();
            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                VKUtilities::TransitionImageLayout(images[i], m_ColorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
                VKTexture2D* swapImageTexture = new VKTexture2D(images[i], imageView[i]);
                m_SwapchainImageTextures.push_back(swapImageTexture);
            }

            // Create the sync primitives for each frame + Command Buffers
            createFrameData();
        }

        void VKSwapchain::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Delete the frame data
            for (auto& frame: m_Frames) {
                //frame.mainCommandBuffer->Reset();
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.presentSemaphore, nullptr);
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.renderSemaphore, nullptr);
                vkDestroyFence(VKDevice::Get().getDevice(), frame.renderFence->getVKFence(), nullptr);
            }

            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
                tex->Release(false);
            }
            m_SwapchainImageTextures.clear();
            vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
        }

        void VKSwapchain::Flip()
        {
        }

        void VKSwapchain::OnResize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_Width == width && m_Height == height)
                return;

            m_IsResized  = true;
            m_IsResizing = true;

            //  Wait for the device to be done executing all the commands
            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            m_Width  = width;
            m_Height = height;

            m_OldSwapChain = m_Swapchain;

            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
                tex->Release(false);
            }
            m_SwapchainImageTextures.clear();

            m_Swapchain = VK_NULL_HANDLE;

            Init(width, height);
            m_IsResizing = false;
        }

        void VKSwapchain::querySwapSurfaceProperties()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the surface capabilities
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &m_SwapSurfaceProperties.capabilities);

            // Get the surface formats supported
            u32 formatsCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, nullptr);
            m_SwapSurfaceProperties.formats.resize(formatsCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &formatsCount, m_SwapSurfaceProperties.formats.data());

            // Get the available present modes
            u32 presentModesCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, nullptr);
            m_SwapSurfaceProperties.presentModes.resize(presentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().getGPU(), VKContext::Get()->getSurface(), &presentModesCount, m_SwapSurfaceProperties.presentModes.data());
        }

        VkSurfaceFormatKHR VKSwapchain::chooseSurfaceFomat()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the right color space
            // Get the right image format for the swapchain images to present mode
            for (const auto& format: m_SwapSurfaceProperties.formats) {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_ColorFormat = format.format;
                    return format;
                }
            }
            return m_SwapSurfaceProperties.formats[0];
        }

        VkPresentModeKHR VKSwapchain::choosePresentMode()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Choose the right kind of image presentation mode for the  swapchain images
            for (const auto& presentMode: m_SwapSurfaceProperties.presentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return presentMode;
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D VKSwapchain::chooseSwapExtent()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // choose the best Swapchain resolution to present the image onto
            if (m_SwapSurfaceProperties.capabilities.currentExtent.width != UINT32_MAX)
                return m_SwapSurfaceProperties.capabilities.currentExtent;
            else {
                auto& capabilities = m_SwapSurfaceProperties.capabilities;
                int   width, height;
                glfwGetFramebufferSize((GLFWwindow*) VKContext::Get()->getWindow()->GetNativeWindow(), &width, &height);

                VkExtent2D actualExtent =
                    {
                        static_cast<u32>(width),
                        static_cast<u32>(height)};

                actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        void VKSwapchain::createSwapchain()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the swapchain image count
            m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.minImageCount + 1;    // For triple buffering
            // Bound checking the swapchain image count only for triple buffer aka 2 frames in flight
            if (m_SwapSurfaceProperties.capabilities.maxImageCount > 0 && m_SwapchainImageCount > m_SwapSurfaceProperties.capabilities.maxImageCount)
                m_SwapchainImageCount = m_SwapSurfaceProperties.capabilities.maxImageCount;
            RAZIX_CORE_TRACE("[Vulkan] Swap images count : %d", m_SwapchainImageCount);

            // Now create the Swapchain
            VkSwapchainCreateInfoKHR swcCI                            = {};
            swcCI.sType                                               = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swcCI.surface                                             = VKContext::Get()->getSurface();
            swcCI.minImageCount                                       = m_SwapchainImageCount;
            swcCI.imageFormat                                         = m_SurfaceFormat.format;
            swcCI.imageColorSpace                                     = m_SurfaceFormat.colorSpace;
            swcCI.imageExtent                                         = m_SwapchainExtent;
            swcCI.imageArrayLayers                                    = 1;
            swcCI.imageUsage                                          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            VKPhysicalDevice::QueueFamilyIndices indices              = VKDevice::Get().getPhysicalDevice().get()->getQueueFamilyIndices();
            u32                             queueFamilyIndices[] = {static_cast<u32>(indices.Graphics), static_cast<u32>(indices.Present)};

            if (indices.Graphics != indices.Present) {
                swcCI.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                swcCI.queueFamilyIndexCount = 2;
                swcCI.pQueueFamilyIndices   = queueFamilyIndices;
            } else {
                swcCI.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                swcCI.queueFamilyIndexCount = 0;          // Optional
                swcCI.pQueueFamilyIndices   = nullptr;    // Optional
            }

            swcCI.preTransform   = m_SwapSurfaceProperties.capabilities.currentTransform;
            swcCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swcCI.clipped        = VK_TRUE;
            swcCI.oldSwapchain   = m_OldSwapChain;

            // Now actually create the swapchainManager
            if (VK_CHECK_RESULT(vkCreateSwapchainKHR(VKDevice::Get().getDevice(), &swcCI, nullptr, &m_Swapchain)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot create swapchain!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Swapchain created successfully!");

            if (m_OldSwapChain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_OldSwapChain, VK_NULL_HANDLE);
                m_OldSwapChain = VK_NULL_HANDLE;
            }
        }

        std::vector<VkImage> VKSwapchain::retrieveSwapchainImages()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkImage> swapImages;

            u32 swapImageCount = 0;
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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkImageView> swapchainImageViews;

            swapchainImageViews.resize(m_SwapchainImageCount);
            for (sz i = 0; i < m_SwapchainImageCount; i++) {
                // Create the image view with the required properties
                VkImageViewCreateInfo imvCI{};
                imvCI.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imvCI.image                           = swapImages[i];
                imvCI.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                imvCI.format                          = m_SurfaceFormat.format;
                imvCI.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                imvCI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                imvCI.subresourceRange.baseMipLevel   = 0;
                imvCI.subresourceRange.levelCount     = 1;
                imvCI.subresourceRange.baseArrayLayer = 0;
                imvCI.subresourceRange.layerCount     = 1;
                if (VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &imvCI, nullptr, &swapchainImageViews[i])))
                    RAZIX_CORE_ERROR("[Vulkan] Cannot create swap image view!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Swap Image view (id= {0} ) succesfully created!", i);
            }
            return swapchainImageViews;
        }

        void VKSwapchain::createFrameData()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                if (!m_Frames[i].renderFence) {
                    VkSemaphoreCreateInfo semaphoreInfo = {};
                    semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                    semaphoreInfo.pNext                 = nullptr;

                    if (m_Frames[i].presentSemaphore == VK_NULL_HANDLE)
                        VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_Frames[i].presentSemaphore));
                    if (m_Frames[i].renderSemaphore == VK_NULL_HANDLE)
                        VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_Frames[i].renderSemaphore));

                    m_Frames[i].renderFence = rzstl::CreateRef<VKFence>(true);
                }
            }
        }

        void VKSwapchain::acquireNextImage(VkSemaphore signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsResizing)
                return;
            u32 nextCmdBufferIndex = (m_CurrentBuffer + 1) % m_SwapchainImageCount;
            {
                auto result = vkAcquireNextImageKHR(VKDevice::Get().getDevice(), m_Swapchain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE, &m_AcquireImageIndex);
                if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                    VK_CHECK_RESULT(result);

                    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                        /*             
                            std::unique_lock<std::mutex> lk(RZApplication::m);
                            RZApplication::halt_execution.wait(lk, [] {
                                return RZApplication::ready_for_execution;
                            });
                        */
                        //OnResize(m_Width, m_Height);
                        vkDeviceWaitIdle(VKDevice::Get().getDevice());
                        //acquireNextImage();
                    }
                    return;
                } else if (result != VK_SUCCESS)
                    RAZIX_CORE_ERROR("[Vulkan] Failed to acquire swap chain image!");

                m_CurrentBuffer = nextCmdBufferIndex;
                return;
            }
        }

        void VKSwapchain::queueSubmit(CommandQueue& commandQueue, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores)
        {
            if (m_IsResizing)
                return;
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& frameData = getCurrentFrameSyncData();

            VkSubmitInfo submitInfo       = {};
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext              = VK_NULL_HANDLE;
            submitInfo.commandBufferCount = static_cast<u32>(commandQueue.size());

            std::vector<VkCommandBuffer> cmdBuffs;
            for (sz i = 0; i < submitInfo.commandBufferCount; i++)
                cmdBuffs.push_back(*((VkCommandBuffer*) commandQueue[i]->getAPIBuffer()));

            submitInfo.pCommandBuffers = cmdBuffs.data();
            std::vector<VkPipelineStageFlags> waitStages;
            for (sz i = 0; i < waitSemaphores.size(); i++)
                waitStages.push_back(VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);

            if (!waitSemaphores.size())
                waitStages.push_back(VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);

            submitInfo.waitSemaphoreCount = static_cast<u32>(waitSemaphores.size());
            submitInfo.pWaitSemaphores    = waitSemaphores.data();
            submitInfo.pWaitDstStageMask  = waitStages.data();

            submitInfo.signalSemaphoreCount = static_cast<u32>(signalSemaphores.size());
            submitInfo.pSignalSemaphores    = signalSemaphores.data();

            frameData.renderFence->reset();

            {
                auto result = vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, frameData.renderFence->getVKFence());
                VK_CHECK_RESULT(result);
#if 0
                if (result == VK_ERROR_DEVICE_LOST) {
                    u32 checkpointDataCount = 0;
                    GetQueueCheckpointDataNV(VKDevice::Get().getGraphicsQueue(), &checkpointDataCount, nullptr);

                    std::vector<VkCheckpointDataNV> checkpointData(checkpointDataCount);
                    for (u32 i = 0; i < checkpointDataCount; i++)
                        checkpointData[i].sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;

                    GetQueueCheckpointDataNV(VKDevice::Get().getGraphicsQueue(), &checkpointDataCount, checkpointData.data());

                    for (auto& data: checkpointData) {
                        RAZIX_CORE_INFO("Checkpoint marker location : {0} | Stage : {1}", data.pCheckpointMarker, data.stage);
                    }
                }
#endif
            }

            frameData.renderFence->wait();
        }

        void VKSwapchain::present(VkSemaphore waitSemaphore)
        {
            if (m_IsResizing)
                return;
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& frameData = getCurrentFrameSyncData();

            auto currentVKImage = static_cast<Graphics::VKTexture2D*>(m_SwapchainImageTextures[m_AcquireImageIndex])->getImage();

            //VKUtilities::TransitionImageLayout(currentVKImage, m_ColorFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            VkPresentInfoKHR present{};
            present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present.pNext              = VK_NULL_HANDLE;
            present.swapchainCount     = 1;
            present.pSwapchains        = &m_Swapchain;
            present.pImageIndices      = &m_AcquireImageIndex;
            present.waitSemaphoreCount = 1;
            present.pWaitSemaphores    = &waitSemaphore;
            present.pResults           = VK_NULL_HANDLE;
            auto error                 = vkQueuePresentKHR(VKDevice::Get().getPresentQueue(), &present);

            //if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR || m_IsResized) {
            //    m_IsResized = !m_IsResized;
            //    //VKContext::Get()->waitIdle();
            //    vkDeviceWaitIdle(VKDevice::Get().getDevice());
            //    for (u32 i = 0; i < m_SwapchainImageCount; i++) {
            //        auto tex = static_cast<RZTexture*>(m_SwapchainImageTextures[i]);
            //        tex->Release(false);
            //        delete m_SwapchainImageTextures[i];
            //    }
            //    m_SwapchainImageTextures.clear();
            //    vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
            //    Init(m_Width, m_Height);
            //    return;
            //}

            if (error == VK_ERROR_OUT_OF_DATE_KHR) {
                vkDeviceWaitIdle(VKDevice::Get().getDevice());
                RAZIX_CORE_ERROR("[Vulkan] Swapchain out of date");
            } else if (error == VK_SUBOPTIMAL_KHR)
                RAZIX_CORE_ERROR("[Vulkan] Swapchain suboptimal");
            else
                VK_CHECK_RESULT(error);
        }
    }    // namespace Graphics
}    // namespace Razix
