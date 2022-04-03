#include "rzxpch.h"
#include "VKImGuiRenderer.h"

#include <imgui/imgui.h>

#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#include <imgui/backends/imgui_impl_vulkan.h>

#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKRenderer.h"

static ImGui_ImplVulkanH_Window g_WindowData;
static VkAllocationCallbacks* g_Allocator = nullptr;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

namespace Razix {
    namespace Graphics {

        VKImGuiRenderer::VKImGuiRenderer(uint32_t width, uint32_t height)
            : m_Framebuffers{}
            , m_Renderpass(nullptr)
            , m_FontTexture(nullptr)
        {
            m_WindowHandle = nullptr;
            m_Width = width;
            m_Height = height;
        }

        void VKImGuiRenderer::setupVulkanWindowData(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
        {
            // Create Descriptor Pool
            {
                VkDescriptorPoolSize pool_sizes[] = {
                    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
                };
                VkDescriptorPoolCreateInfo pool_info = {};
                pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
                pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
                pool_info.pPoolSizes = pool_sizes;
                VkResult err = vkCreateDescriptorPool(VKDevice::Get().getDevice(), &pool_info, g_Allocator, &g_DescriptorPool);
            }

            wd->Surface = surface;
            wd->ClearEnable = true;

            auto swapChain = static_cast<VKSwapchain*>(VKRenderer::getSwapchain());
            wd->Swapchain = swapChain->getSwapchain();
            wd->Width = width;
            wd->Height = height;

            wd->ImageCount = static_cast<uint32_t>(swapChain->GetSwapchainImageCount());

            AttachmentInfo textureTypes[2] = {
                { RZTexture::Type::COLOR, RZTexture::Format::SCREEN }
            };

            Graphics::RenderPassInfo renderpassCI;
            renderpassCI.attachmentCount = 1;
            renderpassCI.textureType = textureTypes;
            renderpassCI.clear = true;

            m_Renderpass = new VKRenderPass(renderpassCI);
            wd->RenderPass = m_Renderpass->getVKRenderPass();

            wd->Frames = (ImGui_ImplVulkanH_Frame*) IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * wd->ImageCount);
            wd->FrameSemaphores = (ImGui_ImplVulkanH_FrameSemaphores*) IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameSemaphores) * wd->ImageCount);
            memset(wd->Frames, 0, sizeof(wd->Frames[0]) * wd->ImageCount);
            memset(wd->FrameSemaphores, 0, sizeof(wd->FrameSemaphores[0]) * wd->ImageCount);

            // Create The Image Views
            {
                for (uint32_t i = 0; i < wd->ImageCount; i++) {
                    auto scBuffer = (VKTexture2D*) swapChain->GetImage(i);
                    wd->Frames[i].Backbuffer = scBuffer->getImage();
                    wd->Frames[i].BackbufferView = scBuffer->getImageView();
                }
            }

            RZTexture::Type attachmentTypes[1];
            attachmentTypes[0] = RZTexture::Type::COLOR;

            RZTexture* attachments[1];
            FramebufferInfo bufferInfo{};
            bufferInfo.width = wd->Width;
            bufferInfo.height = wd->Height;
            bufferInfo.attachmentCount = 1;
            bufferInfo.renderPass = m_Renderpass;
            bufferInfo.screenFBO = true;

            for (uint32_t i = 0; i < VKRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
                attachments[0] = VKRenderer::getSwapchain()->GetImage(i);
                bufferInfo.attachments = attachments;

                m_Framebuffers[i] = new VKFramebuffer(bufferInfo);
                wd->Frames[i].Framebuffer = m_Framebuffers[i]->getVkFramebuffer();
            }
        }

        void VKImGuiRenderer::Init()
        {
            int w, h;
            w = (int) m_Width;
            h = (int) m_Height;
            ImGui_ImplVulkanH_Window* wd = &g_WindowData;
            VkSurfaceKHR surface = VKContext::Get()->getSurface();

            setupVulkanWindowData(wd, surface, w, h);

            // Setup Vulkan binding
            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance = VKContext::Get()->getInstance();
            init_info.PhysicalDevice = VKDevice::Get().getGPU();
            init_info.Device = VKDevice::Get().getDevice();
            init_info.QueueFamily = VKDevice::Get().getPhysicalDevice()->getGraphicsQueueFamilyIndex();
            init_info.Queue = VKDevice::Get().getGraphicsQueue();
            init_info.PipelineCache = VK_NULL_HANDLE;
            init_info.DescriptorPool = g_DescriptorPool;
            init_info.Allocator = g_Allocator;
            init_info.CheckVkResultFn = NULL;
            init_info.MinImageCount = 2;
            init_info.ImageCount = (uint32_t) VKRenderer::getSwapchain()->GetSwapchainImageCount();
            ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
            // Upload Fonts
            {
                ImGuiIO& io = ImGui::GetIO();

                unsigned char* pixels;
                int width, height;
                io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

                RZTexture::Filtering filt(RZTexture::Filtering::FilterMode::NEAREST, RZTexture::Filtering::FilterMode::NEAREST);
                m_FontTexture = new VKTexture2D("ImGui Font Texture", width, height, pixels, RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE, filt);

                VkWriteDescriptorSet write_desc[1] = {};
                write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_desc[0].dstSet = ImGui_ImplVulkanH_GetFontDescriptor();
                write_desc[0].descriptorCount = 1;
                write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_desc[0].pImageInfo = &m_FontTexture->getDescriptor();
                vkUpdateDescriptorSets(VKDevice::Get().getDevice(), 1, write_desc, 0, nullptr);

                io.Fonts->TexID = (ImTextureID) m_FontTexture->GetHandle(); // GetImage();

                ImGui_ImplVulkan_AddTexture(io.Fonts->TexID, ImGui_ImplVulkanH_GetFontDescriptor());

                //ImGui_ImplVulkan_InvalidateFontUploadObjects();
            }
        }

        void VKImGuiRenderer::NewFrame()
        {
            ImGui::NewFrame();
        }

        void VKImGuiRenderer::Render(RZCommandBuffer* commandBuffer)
        {
            ImGui::Render();
            FrameRender(&g_WindowData);
        }

        void VKImGuiRenderer::FrameRender(ImGui_ImplVulkanH_Window* wd)
        {
            wd->FrameIndex = VKRenderer::getSwapchain()->getCurrentImageIndex();

            m_Renderpass->BeginRenderPass(VKRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), m_Framebuffers[wd->FrameIndex], Graphics::SubPassContents::INLINE, wd->Width, wd->Height);

            // Record Imgui Draw Data and draw funcs into command buffer
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), ((VKCommandBuffer*) VKRenderer::getSwapchain()->getCurrentCommandBuffer())->getBuffer());

            m_Renderpass->EndRenderPass(VKRenderer::getSwapchain()->getCurrentCommandBuffer());
        }

        void VKImGuiRenderer::EndFrame()
        {
            
        }

        void VKImGuiRenderer::OnResize(uint32_t width, uint32_t height)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void VKImGuiRenderer::RebuildFontTexture()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }
    }
}