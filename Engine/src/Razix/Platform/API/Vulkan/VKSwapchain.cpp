// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKSwapchain.h"

#include "Razix/Platform/API/Vulkan/VKCommandPool.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"
#include "Razix/Platform/API/Vulkan/VKFence.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glfw/glfw3.h>

namespace Razix {
    namespace Gfx {

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
            m_ImageViews = createSwapImageViews(images);

            // Encapsulate the swapchain images and image views in a RZTexture2D
            m_SwapchainImageTextures.clear();
            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                VKUtilities::TransitionImageLayout(images[i], m_ColorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

                RZHandle<RZTexture> handle;
                void*               where = RZResourceManager::Get().getPool<RZTexture>().obtain(handle);
                new (where) VKTexture(images[i], m_ImageViews[i]);
                IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
                resource->setHandle(handle);
                resource->setName(RZ_SWAP_IMAGE_RES_NAME);

                m_SwapchainImageTextures.push_back(handle);
            }

            // Create the sync primitives for each frame
            createFrameData();
        }

        void VKSwapchain::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            destroyFrameSyncPrimitives();
            DestroyBackBufferImages();

            if (m_Swapchain != VK_NULL_HANDLE)
                vkDestroySwapchainKHR(VKDevice::Get().getDevice(), m_Swapchain, nullptr);
        }

        void VKSwapchain::DestroyBackBufferImages()
        {
            for (size_t i = 0; i < m_ImageViews.size(); i++) {
                auto& view = m_ImageViews[i];
                if (view != VK_NULL_HANDLE) {
                    vkDestroyImageView(VKDevice::Get().getDevice(), view, nullptr);
                    view = VK_NULL_HANDLE;
                }
            }
            m_ImageViews.clear();

            for (u32 i = 0; i < m_SwapchainImageCount; i++)
                RZResourceManager::Get().destroyTexture(m_SwapchainImageTextures[i]);
            m_SwapchainImageTextures.clear();
            m_SwapchainImageCount = static_cast<u32>(m_SwapchainImageTextures.size());
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

            destroyFrameSyncPrimitives();
            DestroyBackBufferImages();

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
            //            if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

            for (const auto& format: m_SwapSurfaceProperties.formats) {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM /*VK_FORMAT_B8G8R8A8_SRGB*/ && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_ColorFormat = format.format;
                    return format;
                }
            }
            return m_SwapSurfaceProperties.formats[0];
        }

        VkPresentModeKHR VKSwapchain::choosePresentMode()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (g_GraphicsFeatures.EnableVSync)
                return VK_PRESENT_MODE_FIFO_KHR;    // VSync

            // Choose the right kind of image presentation mode for the  swapchain images
            for (const auto& presentMode: m_SwapSurfaceProperties.presentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return presentMode;
            }
            return VK_PRESENT_MODE_FIFO_KHR;    // VSync
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
            swcCI.imageUsage                                          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            VKPhysicalDevice::QueueFamilyIndices indices              = VKDevice::Get().getPhysicalDevice().get()->getQueueFamilyIndices();
            u32                                  queueFamilyIndices[] = {static_cast<u32>(indices.Graphics), static_cast<u32>(indices.Present)};

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

        void VKSwapchain::destroyFrameSyncPrimitives()
        {
            for (auto& frame: m_FramesSyncData) {
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.imageAvailableSemaphore, nullptr);
                vkDestroySemaphore(VKDevice::Get().getDevice(), frame.renderingDoneSemaphore, nullptr);
                frame.renderFence.reset();
            }
        }

        void VKSwapchain::createFrameData()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext                 = nullptr;

            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

            for (u32 i = 0; i < m_SwapchainImageCount; i++) {
                VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_FramesSyncData[i].imageAvailableSemaphore));
                VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_FramesSyncData[i].renderingDoneSemaphore));
                //VK_CHECK_RESULT(vkCreateFence(VKDevice::Get().getDevice(), &fenceCreateInfo, nullptr, &m_FramesSyncData[i].renderFence));

                VK_TAG_OBJECT("imageAvailable Semaphore #" + std::to_string(i), VK_OBJECT_TYPE_SEMAPHORE, (uint64_t) m_FramesSyncData[i].imageAvailableSemaphore);
                VK_TAG_OBJECT("renderingDone Semaphore #" + std::to_string(i), VK_OBJECT_TYPE_SEMAPHORE, (uint64_t) m_FramesSyncData[i].renderingDoneSemaphore);
                //VK_TAG_OBJECT("Fence #" + std::to_string(i), VK_OBJECT_TYPE_FENCE, (uint64_t) m_FramesSyncData[i].renderFence);

                m_FramesSyncData[i].renderFence = rzstl::CreateUniqueRef<VKFence>(true);
            }
        }

        u32 VKSwapchain::acquireNextImage(VkSemaphore signalSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsResizing)
                return m_AcquiredBackBufferImageIndex;

            auto& frameData = getCurrentFrameSyncDataVK();

            //{
            //    RAZIX_PROFILE_SCOPEC("vkWaitForFences", RZ_PROFILE_COLOR_GRAPHICS);

            //    VK_CHECK_RESULT(vkWaitForFences(VKDevice::Get().getDevice(), 1, &frameData.renderFence, VK_TRUE, UINT64_MAX));
            //}
            //{
            //    RAZIX_PROFILE_SCOPEC("vkResetFences", RZ_PROFILE_COLOR_GRAPHICS);

            //    VK_CHECK_RESULT(vkResetFences(VKDevice::Get().getDevice(), 1, &frameData.renderFence));
            //}

            frameData.renderFence->wait();
            frameData.renderFence->reset();

            auto result = vkAcquireNextImageKHR(VKDevice::Get().getDevice(), m_Swapchain, UINT64_MAX, frameData.imageAvailableSemaphore, VK_NULL_HANDLE, &m_AcquiredBackBufferImageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                VK_CHECK_RESULT(result);

                if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                    vkDeviceWaitIdle(VKDevice::Get().getDevice());
                    destroyFrameSyncPrimitives();
                    DestroyBackBufferImages();
                    Init(m_SwapchainExtent.width, m_SwapchainExtent.height);

                    VK_CHECK_RESULT(vkAcquireNextImageKHR(VKDevice::Get().getDevice(), m_Swapchain, UINT64_MAX, frameData.imageAvailableSemaphore, VK_NULL_HANDLE, &m_AcquiredBackBufferImageIndex));
                    frameData.renderFence->reset();

                    return m_AcquiredBackBufferImageIndex;
                }
            }
            RAZIX_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan] Failed to acquire swap chain image!");

            return m_AcquiredBackBufferImageIndex;
        }

        void VKSwapchain::queueSubmit(CommandQueue& commandQueue, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores)
        {
            if (m_IsResizing)
                return;
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& frameData = getCurrentFrameSyncDataVK();

            VkSubmitInfo submitInfo       = {};
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext              = VK_NULL_HANDLE;
            submitInfo.commandBufferCount = static_cast<u32>(commandQueue.size());

            std::vector<VkCommandBuffer> cmdBuffs;
            for (sz i = 0; i < submitInfo.commandBufferCount; i++) {
                auto& handle    = commandQueue[i];
                auto  apiBuffer = RZResourceManager::Get().getDrawCommandBufferResource(handle)->getAPIBuffer();
                cmdBuffs.push_back(*((VkCommandBuffer*) apiBuffer));
            }

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

            {
                auto result = vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, frameData.renderFence->getVKFence());
                VK_CHECK_RESULT(result);
#if ENABLE_NV_CHECKPOINT_DEBUGGING
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
        }

        void VKSwapchain::present(VkSemaphore waitSemaphore)
        {
            if (m_IsResizing)
                return;

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkPresentInfoKHR present{};
            present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present.pNext              = VK_NULL_HANDLE;
            present.swapchainCount     = 1;
            present.pSwapchains        = &m_Swapchain;
            present.pImageIndices      = &m_AcquiredBackBufferImageIndex;
            present.waitSemaphoreCount = 1;
            present.pWaitSemaphores    = &waitSemaphore;
            present.pResults           = VK_NULL_HANDLE;
            auto error                 = vkQueuePresentKHR(VKDevice::Get().getPresentQueue(), &present);

            if (error == VK_ERROR_OUT_OF_DATE_KHR) {
                vkDeviceWaitIdle(VKDevice::Get().getDevice());
                RAZIX_CORE_ERROR("[Vulkan] Swapchain out of date");
            } else if (error == VK_SUBOPTIMAL_KHR)
                RAZIX_CORE_ERROR("[Vulkan] Swapchain suboptimal");
            else
                VK_CHECK_RESULT(error);
        }

        void VKSwapchain::submitGraphicsAndFlip(CommandQueue& commandQueue)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Submit and Flip the Graphics Queue
            VkResult result;

            auto& frameData = getCurrentFrameSyncDataVK();

            //----------------------------------------------------------
            // SUBMITING CMD BUFFERS
            //----------------------------------------------------------

            VkSubmitInfo submitInfo       = {};
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext              = VK_NULL_HANDLE;
            submitInfo.commandBufferCount = static_cast<u32>(commandQueue.size());
            // !TODO: OPTIMIZE THIS SHITTY SNIPPET!!!
            std::vector<VkCommandBuffer> cmdBuffs;
            for (sz i = 0; i < submitInfo.commandBufferCount; i++) {
                auto& handle    = commandQueue[i];
                auto  apiBuffer = RZResourceManager::Get().getDrawCommandBufferResource(handle)->getAPIBuffer();
                cmdBuffs.push_back(*((VkCommandBuffer*) apiBuffer));
            }
            submitInfo.pCommandBuffers = cmdBuffs.data();

            // We can do a VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT (will still need memory barriers) which will stall the GPU until all the work is done
            // but this will make memory and pipeline barriers redundant and make async command buffer processing less efficient
            // This should only wait for the final presentation target to finish writing to the final color target!
            VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

            // Submit for work as soon as we have a image to swapchain image to render onto
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores    = &frameData.imageAvailableSemaphore;
            submitInfo.pWaitDstStageMask  = waitStages;

            // Signal that the rendering is done for the presentation engine to resume flip and presenting to the screen
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &frameData.renderingDoneSemaphore;

            // Submit the command buffers for execution and signal a fence to let the CPU know when the submitted command buffers finish execution so that they can be re-used for recording
            result = vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, frameData.renderFence->getVKFence());
            VK_CHECK_RESULT(result);

            //----------------------------------------------------------
            // PRESENTATION
            //----------------------------------------------------------

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.pNext              = VK_NULL_HANDLE;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &m_Swapchain;
            presentInfo.pImageIndices      = &m_AcquiredBackBufferImageIndex;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &frameData.renderingDoneSemaphore;
            presentInfo.pResults           = VK_NULL_HANDLE;

            result = vkQueuePresentKHR(VKDevice::Get().getPresentQueue(), &presentInfo);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                vkDeviceWaitIdle(VKDevice::Get().getDevice());
                RAZIX_CORE_ERROR("[Vulkan] Swapchain out of date");
            } else if (result == VK_SUBOPTIMAL_KHR)
                RAZIX_CORE_ERROR("[Vulkan] Swapchain suboptimal");
            else
                VK_CHECK_RESULT(result);

            m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_SwapchainImageCount;
        }
    }    // namespace Gfx
}    // namespace Razix
