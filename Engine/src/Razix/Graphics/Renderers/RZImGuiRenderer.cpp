#include "rzxpch.h"
#include "RZImGuiRenderer.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "razix/Core/RZApplication.h"

#include <imgui/backends/imgui_impl_glfw.h>


namespace Razix {
    namespace Graphics {

        static glm::vec4 U32ColorToRGBA(uint32_t color)
        {
            float CPa = float((color & 0xff000000UL) >> 24) / 255.0f;
            float CPb = float((color & 0x00ff0000UL) >> 16) / 255.0f;
            float CPg = float((color & 0x0000ff00UL) >> 8) / 255.0f;
            float CPr = float((color & 0x000000ffUL)) / 255.0f;

            return glm::vec4(CPr, CPg, CPb, CPa);
        }

        void RZImGuiRenderer::init()
        {
            // Load the ImGui shaders
            m_UIShader = RZShader::Create("//RazixContent/Shaders/Razix/imgui.rzsf");

            // Configure ImGui
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO(); (void) io;

            // Configure ImGui flags
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            // Setup Dear ImGui style
            // Color scheme - Red like Sascha Willems samples (https://github.com/SaschaWillems/Vulkan/blob/91958acad2c15f52bda74c58f6c39bd980207d2a/base/VulkanUIOverlay.cpp#L31)
            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_TitleBg]          = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
            style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_Header]           = ImVec4(0.8f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_HeaderActive]     = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_CheckMark]        = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_SliderGrab]       = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
            style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
            style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
            style.Colors[ImGuiCol_Button]           = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
            style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
            style.Colors[ImGuiCol_ButtonActive]     = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);

            // Upload the fonts to the GPU
            uploadUIFont("//RazixContent/Fonts/FiraCode/FiraCode-Light.ttf");

            // Now create the descriptor set that will be bound for the shaders
            auto& setInfos = m_UIShader->getSetInfos();
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) 
                            descriptor.texture = m_FontAtlasTexture;
                }
                m_FontAtlasDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
            }

            ImFontAtlas* atlas = io.Fonts;
            // As ocornut mentioned we pass an engine abstracted object and bind it when doing stuff outselves
            ImTextureID set = &m_FontAtlasDescriptorSet;
            atlas->SetTexID(set);

            ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
        }

        void RZImGuiRenderer::createPipeline(RZRenderPass& renderpass)
        {
            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass             = &renderpass;
            pipelineInfo.shader                 = m_UIShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;

            if(m_UIShader)
                m_ImGuiPipeline = Graphics::RZPipeline::Create(pipelineInfo);

            layout.push<glm::vec2>("inPos");
            layout.push<glm::vec2>("inUV");
            layout.push<uint32_t>("inColor");
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
            ImDrawData* imDrawData = ImGui::GetDrawData();
            bool updateCmdBuffers = false;

            if (!imDrawData) { return false; };

            size_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
            size_t indexBufferSize = imDrawData->TotalIdxCount;

            // Update buffers only if vertex or index count has been changed compared to current buffer size
            if ((vertexBufferSize == 0) || (indexBufferSize == 0))
                return false;

            //ImDrawVert* vtxDst = new ImDrawVert[vertexBufferSize];
            //ImDrawIdx* idxDst = new ImDrawIdx[indexBufferSize];

            m_VertexData.clear();
            //m_VertexData.resize(imDrawData->TotalVtxCount);
            m_IndexData.clear();
            m_IndexData.resize(indexBufferSize);
            for (int n = 0; n < imDrawData->CmdListsCount; n++) {
                const ImDrawList* cmd_list = imDrawData->CmdLists[n];

                //memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                //memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                //vtxDst += cmd_list->VtxBuffer.Size;
                //idxDst += cmd_list->IdxBuffer.Size;

                // Get the vertex and index data from here
                //memcpy(&m_VertexData[0], cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

                for (size_t i = 0; i < cmd_list->VtxBuffer.Size; i++) {
                    ImGuiVertex vtx{};
                    vtx.pos = glm::vec2(cmd_list->VtxBuffer.Data[i].pos.x, cmd_list->VtxBuffer.Data[i].pos.y);
                    vtx.uv = glm::vec2(cmd_list->VtxBuffer.Data[i].uv.x, cmd_list->VtxBuffer.Data[i].uv.y);
                    vtx.color = U32ColorToRGBA(cmd_list->VtxBuffer.Data[i].col);
                    m_VertexData.push_back(vtx);
                }
                memcpy(&m_IndexData[0], cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            }

            if (vertexCount != imDrawData->TotalVtxCount) {
                if(m_ImGuiVBO)
                    m_ImGuiVBO->Destroy();
                m_ImGuiVBO = RZVertexBuffer::Create(vertexBufferSize, &m_VertexData[0], BufferUsage::STATIC, "ImGUi VBO");
                updateCmdBuffers = true;
            }

            if (indexCount != imDrawData->TotalIdxCount) {
                if (m_ImGuiIBO)
                    m_ImGuiIBO->Destroy();
                m_ImGuiIBO = RZIndexBuffer::Create(&m_IndexData[0], indexBufferSize, "ImGui IBO");
                updateCmdBuffers = true;
            }

            return updateCmdBuffers;
        }

        void RZImGuiRenderer::draw(RZCommandBuffer* cmdBuffer)
        {
            ImDrawData* imDrawData = ImGui::GetDrawData();
            int32_t vertexOffset = 0;
            int32_t indexOffset = 0;

            if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
                return;
            }

            ImGuiIO& io = ImGui::GetIO();

            // Bind the pipeline and descriptor sets
            m_ImGuiPipeline->Bind(cmdBuffer);

            RZAPIRenderer::BindDescriptorSets(m_ImGuiPipeline, cmdBuffer, &m_FontAtlasDescriptorSet, 1);

            // Update the push constants
            pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            pushConstBlock.translate = glm::vec2(-1.0f);

            RZAPIRenderer::BindPushConstants(m_ImGuiPipeline, cmdBuffer, sizeof(PushConstBlock), &pushConstBlock);

            // Bind the vertex and index buffers
            m_ImGuiVBO->Bind(cmdBuffer);
            m_ImGuiIBO->Bind(cmdBuffer);

            for (uint32_t i = 0; i < imDrawData->CmdListsCount; ++i) {
                const ImDrawList* cmd_list = imDrawData->CmdLists[i];
                for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                    // pcmd->GetTexID(); // Use this to bind the appropriate descriptor set
                    /*
                    VkRect2D scissorRect{};
                    scissorRect.offset.x = std::max((int32_t) (pcmd->ClipRect.x), 0);
                    scissorRect.offset.y = std::max((int32_t) (pcmd->ClipRect.y), 0);
                    scissorRect.extent.width = (uint32_t) (pcmd->ClipRect.z - pcmd->ClipRect.x);
                    scissorRect.extent.height = (uint32_t) (pcmd->ClipRect.w - pcmd->ClipRect.y);
                    vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
                    */
                    //vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                    RZAPIRenderer::DrawIndexed(cmdBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                    indexOffset += pcmd->ElemCount;
                }
                vertexOffset += cmd_list->VtxBuffer.Size;
            }
        }

        void RZImGuiRenderer::uploadUIFont(const std::string& fontPath)
        {
            std::string physicalPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath(fontPath, physicalPath);

            ImGuiIO& io = ImGui::GetIO(); (void) io;
            io.Fonts->AddFontFromFileTTF(physicalPath.c_str(), 16.0f);

            unsigned char* fontData;
            int texWidth, texHeight;
            io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
            size_t uploadSize = texWidth * texHeight * 4 * sizeof(char);

            m_FontAtlasTexture = RZTexture2D::Create("ImGui Font Atlas", texWidth, texHeight, fontData, RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE);
        }
    }
}