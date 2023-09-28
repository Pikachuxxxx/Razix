// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImGuiRenderer.h"

#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"
#include "razix/Core/RZApplication.h"

#include "Razix/Core/RZCPUMemoryManager.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZFramebuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"
#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

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
            m_OverrideGlobalRHIShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ImGui);

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
            // FIXME: custom font is not working when using multiple fonts in vulkan
            //uploadUIFont("//RazixContent/Fonts/FiraCode/FiraCode-Light.ttf");

            // Now create the descriptor set that will be bound for the shaders
            auto setInfos = RZResourceManager::Get().getShaderResource(m_OverrideGlobalRHIShader)->getDescriptorsPerHeapMap();

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
            //sz uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZResourceManager::Get().createTexture({.name = "Awesome Font Icon Atlas", .width = (u32) texWidth, .height = (u32) texHeight, .data = fontData, .type = TextureType::Texture_2D, .format = TextureFormat::RGBA8, .wrapping = Wrapping::CLAMP_TO_EDGE});

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined)
                        descriptor.texture = m_FontAtlasTexture;
                }
                m_FontAtlasDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGui Font Atlas Desc Set"));
            }

            io.Fonts->Build();

            ImFontAtlas* atlas = io.Fonts;
            // As ocornut mentioned we pass an engine abstracted object and bind it when doing stuff ourselves
            ImTextureID set_2 = m_FontAtlasDescriptorSet;
            atlas[0].SetTexID(set_2);

            initDisposableResources();

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN)
                ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
            else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
                ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
                cstr glsl_version = "#version 410";
                ImGui_ImplOpenGL3_Init(glsl_version);
                return;
            }

            m_ImGuiVBO = RZVertexBuffer::Create(Mib(4), nullptr, BufferUsage::Dynamic RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGUi VBO"));
            m_ImGuiIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("ImGui IBO") nullptr, Mib(4) * 6);
        }

        void RZImGuiRenderer::Begin(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            RAZIX_MARK_BEGIN("ImGui Pass", glm::vec4(1.0f, 7.0f, 0.0f, 1.0f));

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;

            ImDrawData* imDrawData       = ImGui::GetDrawData();
            bool        updateCmdBuffers = false;

            if (!imDrawData) { return; };

            u32 vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
            u32 indexBufferSize  = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

            // Update buffers only if vertex or index count has been changed compared to current buffer size
            if ((vertexBufferSize == 0) || (indexBufferSize == 0))
                return;

            /* if ((dynamic_cast<VKVertexBuffer*>(m_ImGuiVBO)->getBuffer() == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount)) {*/
            //m_ImGuiVBO->Destroy();
            //delete m_ImGuiVBO;
            //m_ImGuiVBO = RZVertexBuffer::Create(vertexBufferSize, nullptr, BufferUsage::DYNAMIC RZ_DEBUG_NAME_TAG_STR_E_ARG("ImGUi VBO"));
            //vertexCount = imDrawData->TotalVtxCount;
            //m_ImGuiVBO->UnMap();
            m_ImGuiVBO->Map();
            updateCmdBuffers = true;
            //}

            //if ((dynamic_cast<VKIndexBuffer*>(m_ImGuiIBO)->getBuffer() == VK_NULL_HANDLE) || (indexCount != imDrawData->TotalIdxCount)) {
            //m_ImGuiIBO->Destroy();
            //delete m_ImGuiIBO;
            //m_ImGuiIBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("ImGui IBO") nullptr, imDrawData->TotalIdxCount, BufferUsage::DYNAMIC);
            //indexCount = imDrawData->TotalIdxCount;
            //m_ImGuiIBO->UnMap();
            m_ImGuiIBO->Map();
            updateCmdBuffers = true;

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

            m_ImGuiVBO->UnMap();
            m_ImGuiIBO->UnMap();
        }

        void RZImGuiRenderer::Draw(RZCommandBuffer* cmdBuffer)
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
            RHI::BindPipeline(m_Pipeline, cmdBuffer);

            // Update the push constants
            pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            //pushConstBlock.scale     = glm::vec2(2.0f / ResolutionToExtentsMap[Resolution::k1440p].x, 2.0f / ResolutionToExtentsMap[Resolution::k1440p].y);
            pushConstBlock.translate = glm::vec2(-1.0f);

            RHI::SetViewport(cmdBuffer, 0, 0, (u32) io.DisplaySize.x, (u32) io.DisplaySize.y);
            //RHI::SetViewport(cmdBuffer, 0, 0, (u32) ResolutionToExtentsMap[Resolution::k1440p].x, (u32) ResolutionToExtentsMap[Resolution::k1440p].y);

            RZPushConstant model{};    //RZResourceManager::Get().getShaderResource(m_OverrideGlobalRHIShader)->getPushConstants()[0];
            model.name        = "ImGui model mat";
            model.shaderStage = ShaderStage::Vertex;
            model.size        = sizeof(PushConstBlock);
            model.data        = &pushConstBlock;

            RHI::BindPushConstant(m_Pipeline, cmdBuffer, model);

            // Bind the vertex and index buffers
            m_ImGuiVBO->Bind(cmdBuffer);
            m_ImGuiIBO->Bind(cmdBuffer);

            for (u32 i = 0; i < (u32) imDrawData->CmdListsCount; ++i) {
                ImDrawList* cmd_list = imDrawData->CmdLists[i];
                ImGuizmo::SetDrawlist(cmd_list);
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                    // pcmd->GetTexID(); // Use this to bind the appropriate descriptor set
                    RZDescriptorSet* set = (RZDescriptorSet*) pcmd->TextureId;
                    RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, set, 0);
                    // TODO: Fix this for Vulkan
                    VkCommandBuffer* cmdBuf = (VkCommandBuffer*) (cmdBuffer->getAPIBuffer());

                    RZEngine::Get().GetStatistics().NumDrawCalls++;
                    RZEngine::Get().GetStatistics().IndexedDraws++;

                    // BUG: See this is fine because ImGui is same for the eternity, it's not some crucial thing and won't even make the final game
                    // So I don't see putting such hacky stuff in here, I don't want to be a bitch about making everything super decoupled,
                    // When life gives you oranges that taste like lemonade you still consume them, this doesn't affect the performance at all
                    // Just deal with this cause everything else was done manually, we'll see if this is a issue when we use multi-viewports, until then Cyao BITCH!!!
                    RHI::SetScissorRect(cmdBuffer, std::max((int32_t) (pcmd->ClipRect.x), 0), std::max((int32_t) (pcmd->ClipRect.y), 0), (u32) (pcmd->ClipRect.z - pcmd->ClipRect.x), (u32) (pcmd->ClipRect.w - pcmd->ClipRect.y));
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

            RHI::EndRendering(Graphics::RHI::GetCurrentCommandBuffer());

            RAZIX_MARK_END();
        }

        void RZImGuiRenderer::Resize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;

            RZResourceManager::Get().destroyPipeline(m_Pipeline);

            Graphics::RHI::OnResize(width, height);
        }

        void RZImGuiRenderer::Destroy()
        {
            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;
            //m_DepthTexture->Release(true);

            if (m_FontAtlasDescriptorSet)
                m_FontAtlasDescriptorSet->Destroy();
            //m_FontAtlasTexture->Release(true);
            RZResourceManager::Get().destroyTexture(m_FontAtlasTexture);
            m_ImGuiVBO->Destroy();
            m_ImGuiIBO->Destroy();
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        void RZImGuiRenderer::initDisposableResources()
        {
            // Create the graphics pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Pipeline.ImGui";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);
        }

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
            //sz uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZResourceManager::Get().createTexture({.name = "Awesome Font Icon Atlas", .width = (u32) texWidth, .height = (u32) texHeight, .data = fontData, .type = TextureType::Texture_2D, .format = TextureFormat::RGBA8, .wrapping = Wrapping::CLAMP_TO_EDGE});
        }
    }    // namespace Graphics
}    // namespace Razix
