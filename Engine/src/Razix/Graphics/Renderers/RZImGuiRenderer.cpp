// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImGuiRenderer.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Utilities/RZTimestep.h"

// Imgui
#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui_internal.h>

#ifdef RAZIX_RENDER_API_VULKAN
    #include <vulkan/vulkan.h>
#endif

#include <imgui/backends/imgui_impl_opengl3.h>

namespace Razix {
    namespace Graphics {

        //static glm::vec4 U32ColorToRGBA(uint32_t color)
        //{
        //    float CPa = float((color & 0xff000000UL) >> 24) / 255.0f;
        //    float CPb = float((color & 0x00ff0000UL) >> 16) / 255.0f;
        //    float CPg = float((color & 0x0000ff00UL) >> 8) / 255.0f;
        //    float CPr = float((color & 0x000000ffUL)) / 255.0f;
        //
        //    return glm::vec4(CPr, CPg, CPb, CPa);
        //}

        void RZImGuiRenderer::init()
        {
            // Load the ImGui shaders
            m_UIShader = RZShader::Create("//RazixContent/Shaders/Razix/imgui.rzsf");

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

            // Upload the fonts to the GPU
            uploadUIFont("//RazixContent/Fonts/FiraCode/FiraCode-Light.ttf");

            // Now create the descriptor set that will be bound for the shaders
            auto& setInfos = m_UIShader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                        descriptor.texture = m_FontAtlasTexture;
                }
                m_FontAtlasDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second);
            }

            ImFontAtlas* atlas = io.Fonts;
            // As ocornut mentioned we pass an engine abstracted object and bind it when doing stuff ourselves
            ImTextureID set = m_FontAtlasDescriptorSet;
            atlas->SetTexID(set);

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN)
                ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
            else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
                ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
                const char* glsl_version = "#version 410";
                ImGui_ImplOpenGL3_Init(glsl_version);
                return;
            }

            m_ImGuiVBO = RZVertexBuffer::Create(10, nullptr, BufferUsage::DYNAMIC, "ImGUi VBO");
            //m_ImGuiVBO->Destroy();
            m_ImGuiIBO = RZIndexBuffer::Create(nullptr, 10, "ImGui IBO");
            //m_ImGuiIBO->Destroy();
        }

        void RZImGuiRenderer::createPipeline(RZRenderPass& renderpass)
        {
            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass          = &renderpass;
            pipelineInfo.shader              = m_UIShader;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;

            if (m_UIShader)
                m_ImGuiPipeline = Graphics::RZPipeline::Create(pipelineInfo);

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

        bool RZImGuiRenderer::update(const RZTimestep& dt)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return true;

            ImDrawData* imDrawData       = ImGui::GetDrawData();
            bool        updateCmdBuffers = false;

            if (!imDrawData) { return false; };

            size_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
            size_t indexBufferSize  = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

            // Update buffers only if vertex or index count has been changed compared to current buffer size
            if ((vertexBufferSize == 0) || (indexBufferSize == 0))
                return false;

#if 1
            /* if ((dynamic_cast<VKVertexBuffer*>(m_ImGuiVBO)->getBuffer() == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount)) {*/
            m_ImGuiVBO->UnMap();
            m_ImGuiVBO->Destroy();
            m_ImGuiVBO = RZVertexBuffer::Create(vertexBufferSize, nullptr, BufferUsage::DYNAMIC, "ImGUi VBO");
            //vertexCount = imDrawData->TotalVtxCount;
            //m_ImGuiVBO->UnMap();
            m_ImGuiVBO->Map();
            updateCmdBuffers = true;
            //}

            //if ((dynamic_cast<VKIndexBuffer*>(m_ImGuiIBO)->getBuffer() == VK_NULL_HANDLE) || (indexCount != imDrawData->TotalIdxCount)) {
            m_ImGuiIBO->UnMap();
            m_ImGuiIBO->Destroy();
            m_ImGuiIBO = RZIndexBuffer::Create(nullptr, imDrawData->TotalIdxCount, "ImGui IBO", BufferUsage::DYNAMIC);
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

            m_ImGuiVBO->Flush();
            m_ImGuiIBO->Flush();

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

            return updateCmdBuffers;
        }

        void RZImGuiRenderer::draw(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                return;
            }

            ImDrawData* imDrawData   = ImGui::GetDrawData();
            int32_t     vertexOffset = 0;
            int32_t     indexOffset  = 0;

            if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
                return;
            }

            ImGuiIO& io = ImGui::GetIO();

            // Bind the pipeline and descriptor sets
            m_ImGuiPipeline->Bind(cmdBuffer);

            RZAPIRenderer::BindDescriptorSets(m_ImGuiPipeline, cmdBuffer, &m_FontAtlasDescriptorSet, 1);

            // Update the push constants
            pushConstBlock.scale     = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            pushConstBlock.translate = glm::vec2(-1.0f);

            RZPushConstant& model = m_UIShader->getPushConstants()[0];

            model.size = sizeof(PushConstBlock);
            model.data = &pushConstBlock;

            RZAPIRenderer::BindPushConstant(m_ImGuiPipeline, cmdBuffer, model);

            // Bind the vertex and index buffers
            m_ImGuiVBO->Bind(cmdBuffer);
            m_ImGuiIBO->Bind(cmdBuffer);

            for (uint32_t i = 0; i < imDrawData->CmdListsCount; ++i) {
                const ImDrawList* cmd_list = imDrawData->CmdLists[i];
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                    // pcmd->GetTexID(); // Use this to bind the appropriate descriptor set
                    RZDescriptorSet* set = (RZDescriptorSet*) pcmd->TextureId;
                    RZAPIRenderer::BindDescriptorSets(m_ImGuiPipeline, cmdBuffer, &set, 1);
                    // TODO: Fix this for Vulkan
                    VkCommandBuffer* cmdBuf = (VkCommandBuffer*) (cmdBuffer->getAPIBuffer());

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

        void RZImGuiRenderer::destroy()
        {
            if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
                return;

            m_UIShader->Destroy();
            m_FontAtlasDescriptorSet->Destroy();
            m_FontAtlasTexture->Release(true);
            m_ImGuiVBO->Destroy();
            m_ImGuiIBO->Destroy();
            m_ImGuiPipeline->Destroy();
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
            //size_t uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZTexture2D::Create("ImGui Font Atlas", texWidth, texHeight, fontData, RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE);
        }
    }    // namespace Graphics
}    // namespace Razix