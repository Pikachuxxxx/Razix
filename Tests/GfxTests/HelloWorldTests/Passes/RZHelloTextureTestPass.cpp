#include "RZHelloTextureTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTextureTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            RZShaderDesc desc = {};
            desc.filePath     = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.HelloTextureTest.rzsf";
            desc.libraryID    = ShaderBuiltin::Default;
            desc.name         = "HelloTexture";
            m_Shader          = RZResourceManager::Get().createShader(desc);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.HelloTexture";
            pipelineInfo.shader                 = m_Shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
            pipelineInfo.depthFormat            = TextureFormat::DEPTH16_UNORM;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Import a test texture
            RZTextureDesc testTexDesc{};
            testTexDesc.name       = "TestCheckerTexture";
            testTexDesc.enableMips = false;
            testTexDesc.filePath   = "//RazixContent/Textures/TestCheckerMap.png";
            testTexDesc.flipY      = false;
            m_TestTextureHandle    = RZResourceManager::Get().createTexture(testTexDesc);

            struct HelloTexturePassData
            {
                RZFrameGraphResource Depth;
            };

            framegraph.addCallbackPass<HelloTexturePassData>(
                "[Test] Pass.Builtin.Code.HelloTexture",
                [&](HelloTexturePassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name                  = "SceneDepth";
                    depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type                  = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                             = builder.create<RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);

                    data.Depth = builder.write(data.Depth);
                },
                [=](const HelloTexturePassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Texture Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.HelloTexture", Utilities::GenerateHashedColor4(22u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentBackBufferImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment  = {resources.get<RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind descriptor sets and resources
                    if (RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(m_Shader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["g_TestTexture"];    // Must match the name in shader
                        // TODO: Sampler is static and set on the RootSignature Directly
                        if (descriptor)
                            descriptor->texture = m_TestTextureHandle;

                        // Vulkan will create a default sampler if not found
                        RZResourceManager::Get().getShaderResource(m_Shader)->updateBindVarsHeaps();
                    }

                    auto& sceneDrawParams = Gfx::RZResourceManager::Get().getShaderResource(m_Shader)->getSceneDrawParams();
                    Gfx::RHI::BindUserDescriptorSets(m_Pipeline, cmdBuffer, sceneDrawParams.userSets, 0);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Draw 3 vertices
                    Gfx::RHI::Draw(cmdBuffer, 3);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTextureTestPass::destroy()
        {
            RZResourceManager::Get().destroyTexture(m_TestTextureHandle);
            RZResourceManager::Get().destroyShader(m_Shader);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
