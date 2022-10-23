// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImGuiRenderer.h"

#include "Razix/Core/RZEngine.h"
#include "razix/Core/RZApplication.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZFramebuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Utilities/RZTimestep.h"

// Imgui
#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui_internal.h>

#ifdef RAZIX_RENDER_API_VULKAN
    #include <vulkan/vulkan.h>
#endif

#include <imgui/backends/imgui_impl_opengl3.h>

#include <imgui/plugins/ImGuizmo.h>

namespace Razix {
    namespace Graphics {

        void RZImGuiRenderer::Init()
        {
            m_RendererName = "ImGui Renderer";

            // Init the width and height of RT
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Load the ImGui shaders
            m_OverrideGlobalRHIShader = RZShaderLibrary::Get().getShader("imgui.rzsf");

            // Configure ImGui
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO();
            (void) io;

            // Configure ImGui flags
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            // Setup Dear ImGui style
            // Color scheme - Red like Sascha Willems samples (https://github.com/SaschaWillems/Vulkan/blob/91958acad2c15f52bda74c58f6c39bd980207d2a/base/VulkanUIOverlay.cpp#L31)
            ImGuiStyle& style                       = ImGui::GetStyle();
            style.Colors[ImGuiCol_TitleBg]          = ImVec4(1.0f, 0.43f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(1.0f, 0.43f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.43f, 0.0f, 0.1f);
            style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(1.0f, 0.43f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_Header]           = ImVec4(0.8f, 0.22f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_HeaderActive]     = ImVec4(1.0f, 0.43f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(1.0f, 0.43f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_CheckMark]        = ImVec4(1.0f, 0.43f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_SliderGrab]       = ImVec4(1.0f, 0.43f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.43f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
            style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
            style.Colors[ImGuiCol_Button]           = ImVec4(1.0f, 0.43f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(1.0f, 0.43f, 0.0f, 0.6f);
            style.Colors[ImGuiCol_ButtonActive]     = ImVec4(1.0f, 0.43f, 0.0f, 0.8f);

            io.Fonts->AddFontDefault();
            // Upload the fonts to the GPU
            //uploadUIFont("//RazixContent/Fonts/FiraCode/FiraCode-Light.ttf");

            // Now create the descriptor set that will be bound for the shaders
            auto& setInfos = m_OverrideGlobalRHIShader->getSetsCreateInfos();
            //for (auto& setInfo: setInfos) {
            //    // Fill the descriptors with buffers and textures
            //    for (auto& descriptor: setInfo.second) {
            //        if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
            //            descriptor.texture = m_FontAtlasTexture;
            //    }
            //    m_FontAtlasDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second);
            //}

            // Add icon fonts to ImGui
            // merge in icons from Font Awesome
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
            ImFontConfig         icons_config;
            icons_config.MergeMode  = true;
            icons_config.PixelSnapH = true;
            std::string trueFontPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/Fonts/" + std::string(FONT_ICON_FILE_NAME_FAS), trueFontPath);
            io.Fonts->AddFontFromFileTTF(trueFontPath.c_str(), 16.0f, &icons_config, icons_ranges);

            unsigned char* fontData;
            int            texWidth, texHeight;
            io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
            //size_t uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("ImGui Font Atlas") "Awesome Font Icon Atlas", texWidth, texHeight, fontData, RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE);

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                        descriptor.texture = m_FontAtlasTexture;
                }
                m_FontAtlasDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG ("ImGui Font Atlas Desc Set"));
            }

            io.Fonts->Build();

            ImFontAtlas* atlas = io.Fonts;
            // As ocornut mentioned we pass an engine abstracted object and bind it when doing stuff ourselves
            ImTextureID set_2 = m_FontAtlasDescriptorSet;
            atlas[0].SetTexID(set_2);


            InitDisposableResources();

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN)
                ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
            else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
                ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
                const char* glsl_version = "#version 410";
                ImGui_ImplOpenGL3_Init(glsl_version);
                return;
            }

            m_ImGuiVBO = RZVertexBuffer::Create(10, nullptr, BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG ("ImGUi VBO"));
            //m_ImGuiVBO->Destroy();
            m_ImGuiIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("ImGui IBO") nullptr, 10);
            //m_ImGuiIBO->Destroy();
        }

        void RZImGuiRenderer::InitDisposableResources()
        {
            // Create the renderpass
            Graphics::AttachmentInfo textureTypes[2] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN, false},
                {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH, true}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.attachmentInfos = textureTypes;
            renderPassInfo.name            = "ImGui UI pass";

            m_RenderPass = Graphics::RZRenderPass::Create(renderPassInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGui Pass"));

            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass          = m_RenderPass;
            pipelineInfo.shader              = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;

            if (m_OverrideGlobalRHIShader)
                m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGui Pipeline"));

            // Framebuffer (we need on per frame ==> 3 in total)
            // Create the framebuffer
            Graphics::RZTexture::Type attachmentTypes[2];
            attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

            auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
            m_DepthTexture    = Graphics::RZDepthTexture::Create(m_ScreenBufferWidth, m_ScreenBufferHeight);

            m_Framebuffers.clear();
            for (uint32_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
                Graphics::RZTexture* attachments[2];
                attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
                attachments[1] = m_DepthTexture;

                Graphics::FramebufferInfo frameBufInfo{};
                frameBufInfo.width           = m_ScreenBufferWidth;
                frameBufInfo.height          = m_ScreenBufferHeight;
                frameBufInfo.attachmentCount = 2;
                frameBufInfo.renderPass      = m_RenderPass;
                frameBufInfo.attachments     = attachments;

                m_Framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGui Renderer FB")));
            }

            // TODO: This is also to be moved to the renderer static initialization
            for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("ImGui Renderer Main Command Buffers"));
            }

            //layout.push<glm::vec2>("inPos");
            //layout.push<glm::vec2>("inUV");
            //layout.push<uint32_t>("inColor");
            /*

            ImDrawData* imDrawData = ImGui::GetDrawData();
            bool updateCmdBuffers = false;
            size_t vertexBufferSize = sizeof(ImDrawVert);
            size_t indexBufferSize = sizeof(ImDrawVert);
            if (imDrawData) {
                vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
                indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);
            }
             
            m_ImGuiVBO = RZVertexBuffer::Create(vertexBufferSize, nullptr, BufferUsage::DYNAMIC, "ImGui VBO");
            m_ImGuiVBO->AddBufferLayout(layout);

            m_ImGuiIBO = RZIndexBuffer::Create(nullptr, indexBufferSize, "ImGui IBO", BufferUsage::DYNAMIC);
            */
        }

        void RZImGuiRenderer::Begin()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Begin recording the command buffers
            //Graphics::RZAPIRenderer::Begin(m_MainCommandBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]);

            // Update the viewport
            Graphics::RZAPIRenderer::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;

            ImDrawData* imDrawData       = ImGui::GetDrawData();
            bool        updateCmdBuffers = false;

            if (!imDrawData) { return; };

            uint32_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
            uint32_t indexBufferSize  = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

            // Update buffers only if vertex or index count has been changed compared to current buffer size
            if ((vertexBufferSize == 0) || (indexBufferSize == 0))
                return;

#if 1
            /* if ((dynamic_cast<VKVertexBuffer*>(m_ImGuiVBO)->getBuffer() == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount)) {*/
            m_ImGuiVBO->UnMap();
            m_ImGuiVBO->Destroy();
            m_ImGuiVBO = RZVertexBuffer::Create(vertexBufferSize, nullptr, BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG ("ImGUi VBO"));
            //vertexCount = imDrawData->TotalVtxCount;
            //m_ImGuiVBO->UnMap();
            m_ImGuiVBO->Map();
            updateCmdBuffers = true;
            //}

            //if ((dynamic_cast<VKIndexBuffer*>(m_ImGuiIBO)->getBuffer() == VK_NULL_HANDLE) || (indexCount != imDrawData->TotalIdxCount)) {
            m_ImGuiIBO->UnMap();
            m_ImGuiIBO->Destroy();
            m_ImGuiIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("ImGui IBO") nullptr, imDrawData->TotalIdxCount, BufferUsage::DYNAMIC);
            //indexCount = imDrawData->TotalIdxCount;
            //m_ImGuiIBO->UnMap();
            m_ImGuiIBO->Map();
            updateCmdBuffers = true;
            //}`

            // Upload vertex and index data to the GPU
            ImDrawVert* vtxDst = (ImDrawVert*) m_ImGuiVBO->GetMappedBuffer();
            ImDrawIdx*  idxDst = (ImDrawIdx*) m_ImGuiIBO->GetMappedBuffer();

            for (int n = 0; n < imDrawData->CmdListsCount; n++) {
                const ImDrawList* cmd_list = imDrawData->CmdLists[n];

                memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                vtxDst += cmd_list->VtxBuffer.Size;
                idxDst += cmd_list->IdxBuffer.Size;
            }

            //m_ImGuiVBO->Flush();
            //m_ImGuiIBO->Flush();

#endif

#undef VULF_IMGUI_IMPL
#ifdef VULF_IMGUI_IMPL

            // Vertex buffer
            if ((m_ImGuiVBO.get_buffer() == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount)) {
                m_ImGuiVBO.unmap();
                // Destroy the buffer here before creating a new one
                VKLogicalDevice::Get()->createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &m_ImGuiVBO, vertexBufferSize);

                vertexCount = imDrawData->TotalVtxCount;
                m_ImGuiVBO.unmap();
                m_ImGuiVBO.map();
                updateCmdBuffers = true;
            }

            // Index  buffer
            if ((m_ImGuiIBO.get_buffer() == VK_NULL_HANDLE) || (indexCount != imDrawData->TotalIdxCount)) {
                m_ImGuiIBO.unmap();
                // Destroy the buffer here before creating a new one
                VKLogicalDevice::Get()->createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &m_ImGuiIBO, indexBufferSize);

                indexCount = imDrawData->TotalIdxCount;
                m_ImGuiIBO.unmap();
                m_ImGuiIBO.map();
                updateCmdBuffers = true;
            }

            // Upload vertex and index data to the GPU
            ImDrawVert* vtxDst = (ImDrawVert*) m_ImGuiVBO.get_mapped();
            ImDrawIdx*  idxDst = (ImDrawIdx*) m_ImGuiIBO.get_mapped();

            for (int n = 0; n < imDrawData->CmdListsCount; n++) {
                const ImDrawList* cmd_list = imDrawData->CmdLists[n];
                memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                vtxDst += cmd_list->VtxBuffer.Size;
                idxDst += cmd_list->IdxBuffer.Size;
            }

            m_ImGuiVBO.flush();
            m_ImGuiIBO.flush();

#endif
            //return updateCmdBuffers;
        }

        void RZImGuiRenderer::Submit(RZCommandBuffer* cmdBuffer)
        {
            ImDrawData* imDrawData = ImGui::GetDrawData();

            if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
                return;
            }

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                return;
            }

            int32_t vertexOffset = 0;
            int32_t indexOffset  = 0;

            ImGuiIO& io = ImGui::GetIO();

            // Bind the pipeline and descriptor sets
            m_Pipeline->Bind(cmdBuffer);

            // Update the push constants
            pushConstBlock.scale     = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            pushConstBlock.translate = glm::vec2(-1.0f);

            RZPushConstant& model = m_OverrideGlobalRHIShader->getPushConstants()[0];

            model.size = sizeof(PushConstBlock);
            model.data = &pushConstBlock;

            RZAPIRenderer::BindPushConstant(m_Pipeline, cmdBuffer, model);

            // Bind the vertex and index buffers
            m_ImGuiVBO->Bind(cmdBuffer);
            m_ImGuiIBO->Bind(cmdBuffer);

            for (uint32_t i = 0; i < (uint32_t) imDrawData->CmdListsCount; ++i) {
                ImDrawList* cmd_list = imDrawData->CmdLists[i];
                ImGuizmo::SetDrawlist(cmd_list);
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                    // pcmd->GetTexID(); // Use this to bind the appropriate descriptor set
                    RZDescriptorSet* set = (RZDescriptorSet*) pcmd->TextureId;
                    RZAPIRenderer::BindDescriptorSets(m_Pipeline, cmdBuffer, &set, 1);
                    // TODO: Fix this for Vulkan
                    VkCommandBuffer* cmdBuf = (VkCommandBuffer*) (cmdBuffer->getAPIBuffer());

                    RZEngine::Get().GetStatistics().NumDrawCalls++;
                    RZEngine::Get().GetStatistics().IndexedDraws++;

                    // BUG: See this is fine because ImGui is same for the eternity, it's not some crucial thing and won't even make the final game
                    // So I don't see putting such hacky stuff in here, I don't want to be a bitch about making everything super decoupled,
                    // When life gives you oranges that taste like lemonade you still consume them, this doesn't affect the performance at all
                    // Just deal with this cause everything else was done manually, we'll see if this is a issue when we use multi-viewports, until then Cyao BITCH!!!
                    RZAPIRenderer::SetScissorRect(cmdBuffer, std::max((int32_t) (pcmd->ClipRect.x), 0), std::max((int32_t) (pcmd->ClipRect.y), 0), (uint32_t) (pcmd->ClipRect.z - pcmd->ClipRect.x), (uint32_t) (pcmd->ClipRect.w - pcmd->ClipRect.y));
#ifdef RAZIX_RENDER_API_VULKAN
                    if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN)
                        vkCmdDrawIndexed(*cmdBuf, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
#endif
                    //RZAPIRenderer::DrawIndexed(cmdBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                    indexOffset += pcmd->ElemCount;
                }
                vertexOffset += cmd_list->VtxBuffer.Size;
            }
        }

        void RZImGuiRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the render pass and recording
            m_RenderPass->EndRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer());
            // Submit the render queue before presenting next
            Graphics::RZAPIRenderer::Submit(Graphics::RZAPIRenderer::getCurrentCommandBuffer());
        }

        void RZImGuiRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float now = m_RendererTimer.GetElapsedS();
            m_PassTimer.Update(now);
            RZEngine::Get().GetStatistics().ImGuiPass = abs(RZEngine::Get().GetStatistics().DeltaTime - m_PassTimer.GetTimestepMs());

            Graphics::RZAPIRenderer::SubmitWork();
            Graphics::RZAPIRenderer::Present();
        }

        void RZImGuiRenderer::Resize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;
            m_DepthTexture->Release(true);

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_Pipeline->Destroy();
            m_RenderPass->Destroy();

            Graphics::RZAPIRenderer::OnResize(width, height);

            InitDisposableResources();
        }

        void RZImGuiRenderer::Destroy()
        {
            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;
            m_DepthTexture->Release(true);
            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_FontAtlasDescriptorSet->Destroy();
            m_FontAtlasTexture->Release(true);
            m_ImGuiVBO->Destroy();
            m_ImGuiIBO->Destroy();
            m_Pipeline->Destroy();
            m_RenderPass->Destroy();
        }

        void RZImGuiRenderer::OnEvent(RZEvent& event)
        {
        }

        //static glm::vec4 U32ColorToRGBA(uint32_t color)
        //{
        //    float CPa = float((color & 0xff000000UL) >> 24) / 255.0f;
        //    float CPb = float((color & 0x00ff0000UL) >> 16) / 255.0f;
        //    float CPg = float((color & 0x0000ff00UL) >> 8) / 255.0f;
        //    float CPr = float((color & 0x000000ffUL)) / 255.0f;
        //
        //    return glm::vec4(CPr, CPg, CPb, CPa);
        //}

        void RZImGuiRenderer::uploadUIFont(const std::string& fontPath)
        {
            std::string physicalPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath(fontPath, physicalPath);

            ImGuiIO& io = ImGui::GetIO();
            (void) io;
            io.Fonts->AddFontFromFileTTF(physicalPath.c_str(), 16.0f);

            unsigned char* fontData;
            int            texWidth, texHeight;
            io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
            //size_t uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG ("ImGui Font Atlas (path one)") "ImGui Font Atlas", texWidth, texHeight, fontData, RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE);
        }
    }    // namespace Graphics
}    // namespace Razix