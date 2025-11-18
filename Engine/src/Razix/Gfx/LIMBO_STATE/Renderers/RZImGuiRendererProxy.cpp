// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImGuiRendererProxy.h"

#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "razix/Core/App/RZApplication.h"

#include "Razix/Core/Memory/RZCPUMemoryManager.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZSwapchain.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include <vulkan/vulkan.h>
#endif

namespace Razix {
    namespace Gfx {

        void RZImGuiRendererProxy::Init()
        {
            m_RendererName = "ImGuiProxyRenderer";

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            setupImGuiContext();

            setupImGuiFlags();

            setupImGuiStyle();

            setupResources();

            loadImGuiFonts();

            if (Razix::Gfx::RZGraphicsContext::GetRenderAPI() == Razix::Gfx::RenderAPI::VULKAN)
                ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) RZApplication::Get().getWindow()->GetNativeWindow(), true);
        }

        void RZImGuiRendererProxy::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RHI::EndRendering(Gfx::RHI::GetCurrentCommandBuffer());

            RAZIX_MARK_END();
        }

        void RZImGuiRendererProxy::Resize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            RZResourceManager::Get().destroyPipeline(m_Pipeline);

            Gfx::RHI::OnResize(width, height);
        }

        void RZImGuiRendererProxy::Destroy()
        {
            RZResourceManager::Get().destroyIndexBuffer(m_ImGuiIBO);
            RZResourceManager::Get().destroyVertexBuffer(m_ImGuiVBO);
            RZResourceManager::Get().destroyTexture(m_FontAtlasTexture);
            RZResourceManager::Get().destroyDescriptorSet(m_FontAtlasDescriptorSet);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        void RZImGuiRendererProxy::setupImGuiContext()
        {
            // Configure ImGui
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
        }

        void RZImGuiRendererProxy::setupImGuiFlags()
        {
            ImGuiIO& io = ImGui::GetIO();
            (void) io;

            // Configure ImGui flags
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        void RZImGuiRendererProxy::setupImGuiStyle()
        {
            // Setup Dear ImGui style
            // Color scheme - Red like Sascha Willems samples (https://github.com/SaschaWillems/Vulkan/blob/91958acad2c15f52bda74c58f6c39bd980207d2a/base/VulkanUIOverlay.cpp#L31)
            ImGuiStyle& style                       = ImGui::GetStyle();
            style.Colors[ImGuiCol_TitleBg]          = ImVec4(1.0f, 0.43f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(1.0f, 0.43f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.43f, 0.0f, 0.1f);
            style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(0.15f, 0.15f, 0.15f, 0.8f);
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
        }

        void RZImGuiRendererProxy::setupResources()
        {
            m_ImGuiShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ImGui);

            Gfx::RZPipelineDesc pipelineInfo    = {};
            pipelineInfo.name                   = "Pipeline.ImGui";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = m_ImGuiShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthFormat            = Gfx::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            constexpr u32 MaxBufferSize = 16_Mib;

            RZBufferDesc vertexBufferDesc          = {};
            vertexBufferDesc.name                  = "VB_ImGui";
            vertexBufferDesc.usage                 = BufferUsage::PersistentStream;
            vertexBufferDesc.size                  = MaxBufferSize;
            vertexBufferDesc.data                  = NULL;
            vertexBufferDesc.initResourceViewHints = kCBV;
            m_ImGuiVBO                             = RZResourceManager::Get().createVertexBuffer(vertexBufferDesc);

            RZBufferDesc indexBufferDesc          = {};
            indexBufferDesc.name                  = "IB_ImGui";
            indexBufferDesc.usage                 = BufferUsage::PersistentStream;
            indexBufferDesc.count                 = MaxBufferSize;
            indexBufferDesc.data                  = NULL;
            indexBufferDesc.initResourceViewHints = kCBV;
            m_ImGuiIBO                            = RZResourceManager::Get().createIndexBuffer(indexBufferDesc);
        }

        void RZImGuiRendererProxy::loadImGuiFonts()
        {
            ImGuiIO& io = ImGui::GetIO();
            (void) io;

            // Upload the fonts to the GPU
            // FIXME: custom font is not working when using multiple fonts in vulkan
            //uploadUIFont("//RazixContent/Fonts/FiraCode/FiraCode-Light.ttf");

            // Now create the descriptor set that will be bound for the shaders
            auto setInfos = RZResourceManager::Get().getShaderResource(m_ImGuiShader)->getDescriptorsPerHeapMap();

            // Add icon fonts to ImGui
            io.Fonts->AddFontDefault();

            // merge in icons from Font Awesome
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
            ImFontConfig         icons_config;
            icons_config.MergeMode  = true;
            icons_config.PixelSnapH = true;
            // https://github.com/ocornut/imgui/issues/1259
            icons_config.FontDataOwnedByAtlas = true;    // False Not working!!!
            std::string trueFontPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/Fonts/" + std::string(FONT_ICON_FILE_NAME_FAS), trueFontPath);
            io.Fonts->AddFontFromFileTTF(trueFontPath.c_str(), 12.0f, &icons_config, icons_ranges);

            unsigned char* fontData = NULL;
            int            texWidth, texHeight;
            io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
            u32 uploadSize = texWidth * texHeight * 4 * sizeof(char);

            RZTextureDesc imguiFontTextureDesc         = {};
            imguiFontTextureDesc.name                  = "Texture.ImGui.AwesomeFontIconAtlas";
            imguiFontTextureDesc.width                 = (u32) texWidth;
            imguiFontTextureDesc.height                = (u32) texHeight;
            imguiFontTextureDesc.data                  = fontData;
            imguiFontTextureDesc.size                  = uploadSize;
            imguiFontTextureDesc.ownsInitData          = false;
            imguiFontTextureDesc.type                  = TextureType::k2D;
            imguiFontTextureDesc.initResourceViewHints = kSRV;
            imguiFontTextureDesc.format                = TextureFormat::RGBA8;

            m_FontAtlasTexture = RZResourceManager::Get().createTexture(imguiFontTextureDesc);

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Gfx::DescriptorType::kTexture)
                        descriptor.texture = m_FontAtlasTexture;
                    // sampler use a default one
                }

                RZDescriptorSetDesc descSetCreateDesc = {};
                descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                descSetCreateDesc.name                = "DescriptorSet.ImGuiFontAtlas";
                descSetCreateDesc.descriptors         = setInfo.second;
                m_FontAtlasDescriptorSet              = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);
            }

            io.Fonts->Build();

            ImFontAtlas* atlas = io.Fonts;
            // As ocornut mentioned we pass an engine abstracted object and bind it when doing stuff ourselves
            ImTextureID set_2 = &m_FontAtlasDescriptorSet;
            atlas[0].SetTexID(set_2);
        }

        void RZImGuiRendererProxy::uploadCustomUIFont(const std::string& fontPath)
        {
            std::string physicalPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath(fontPath, physicalPath);

            ImGuiIO& io = ImGui::GetIO();
            (void) io;
            io.Fonts->AddFontFromFileTTF(physicalPath.c_str(), 16.0f);

            unsigned char* fontData;
            int            texWidth, texHeight;
            io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

            RZTextureDesc fontAtlasDesc = {};
            fontAtlasDesc.name          = "Texture.2D.ImGuiAwesomeIconAtlas";
            fontAtlasDesc.width         = (u32) texWidth;
            fontAtlasDesc.height        = (u32) texHeight;
            fontAtlasDesc.data          = fontData;
            fontAtlasDesc.type          = TextureType::k2D;
            fontAtlasDesc.format        = TextureFormat::RGBA8;

            m_FontAtlasTexture = RZResourceManager::Get().createTexture(fontAtlasDesc);
        }

    }    // namespace Gfx
}    // namespace Razix
