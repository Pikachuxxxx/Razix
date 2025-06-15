#include "RZWaveIntrinsicsTestPass.h"

namespace Razix {
    namespace Gfx {

        void RZWaveIntrinsicsTestPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            if (!g_GraphicsFeatures.SupportsWaveIntrinsics) {
                RAZIX_CORE_ERROR("[FrameGraph] Wave Intrinsics not supported on this GPU!");
                return;
            }

            // Create the shader and the pipeline
            RZShaderDesc desc = {};
            desc.filePath     = "//TestsRoot/GfxTests/HelloWorldTests/Shaders/Razix/Shader.Test.WaveIntrinsicsTest.rzsf";
            desc.libraryID    = ShaderBuiltin::Default;
            desc.name         = "WaveIntrinsics";
            m_Shader          = RZResourceManager::Get().createShader(desc);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.WaveIntrinsics";
            pipelineInfo.shader                 = m_Shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
            pipelineInfo.depthFormat            = TextureFormat::DEPTH16_UNORM;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            struct WaveIntrinsicsData
            {
                RZFrameGraphResource Depth;
                RZFrameGraphResource DebugBuffer;
            };

            struct WaveIntrinsicsConstantBufferData
            {
                u32 waveMode = 1;
                u32 laneSize = 0;
            };

            framegraph.getBlackboard()
                .add<WaveIntrinsicsData>() = framegraph.addCallbackPass<WaveIntrinsicsData>(
                "[Test] Pass.Builtin.Code.WaveIntrinsics",
                [&](WaveIntrinsicsData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name                  = "SceneDepth";
                    depthTextureDesc.width                 = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height                = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format                = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type                  = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                             = builder.create<RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
                    data.Depth                             = builder.write(data.Depth);
                    //RZBufferDesc DebugDataBufferDesc{};
                    //DebugDataBufferDesc.name  = "WaveIntrinsicsConstantBufferData";
                    //DebugDataBufferDesc.size  = sizeof(SSAOParamsData);
                    //DebugDataBufferDesc.usage = BufferUsage::PersistentStream;
                    //data.DebugBuffer          = builder.create<RZFrameGraphBuffer>(DebugDataBufferDesc.name, CAST_TO_FG_BUF_DESC DebugDataBufferDesc);
                    //
                    //data.DebugBuffer = builder.write(data.DebugBuffer);
                },
                [=](const WaveIntrinsicsData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("[Test] WaveIntrinsics Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.WaveIntrinsics", Utilities::GenerateHashedColor4(45u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentBackBufferImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment = {resources.get<RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Bind the push CB with the render mode
                    // Set the Descriptor Set once rendering starts
                    //if (RZFrameGraph::IsFirstFrame()) {
                    //    auto&         shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();
                    //    RZDescriptor* descriptor     = nullptr;
                    //    descriptor                   = shaderBindVars[resources.getResourceName<RZFrameGraphBuffer>(data.DebugBuffer)];
                    //    if (descriptor)
                    //        descriptor->uniformBuffer = resources.get<RZFrameGraphBuffer>(data.DebugBuffer).getHandle();
                    //    RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    //}
                    //
                    //WaveIntrinsicsConstantBufferData debugData{};
                    //debugData.waveMode = 2;
                    //debugData.laneSize = g_GraphicsFeatures.MinLaneWidth;
                    //
                    //auto DataHandle = resources.get<RZFrameGraphBuffer>(data.DebugBuffer).getHandle();
                    //RZResourceManager::Get().getUniformBufferResource(DataHandle)->SetData(sizeof(WaveIntrinsicsConstantBufferData), &debugData);
                    //
                    //// Bind the descriptor set
                    //auto& sceneDrawParams = Gfx::RZResourceManager::Get().getShaderResource(shader)->getSceneDrawParams();
                    //Gfx::RHI::BindUserDescriptorSets(m_Pipeline, cmdBuffer, sceneDrawParams.userSets, 0);

                    // Draw 3 vertices
                    Gfx::RHI::Draw(cmdBuffer, 3);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZWaveIntrinsicsTestPass::destroy()
        {
            if (g_GraphicsFeatures.SupportsWaveIntrinsics) {
                RZResourceManager::Get().destroyShader(m_Shader);
                RZResourceManager::Get().destroyPipeline(m_Pipeline);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
