// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWaveInstrinsicsTestPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

        void RZWaveInstrinsicsTestPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            if (!g_GraphicsFeatures.SupportsWaveIntrinsics) {
                RAZIX_CORE_ERROR("[FrameGraph] Wave Intrinsics not supported on this GPU!");
                return;
            }

            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::WaveIntrinsicsTest);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.WaveIntrinsics";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
#ifdef __APPLE__    // Metal cannot draw without a depth attachment
            pipelineInfo.depthFormat = TextureFormat::DEPTH16_UNORM;
#endif
            pipelineInfo.cullMode            = Gfx::CullMode::None;
            pipelineInfo.drawType            = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled    = false;
            pipelineInfo.depthWriteEnabled   = false;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            struct WaveIntrinsicsData
            {
                FrameGraph::RZFrameGraphResource Depth;
                FrameGraph::RZFrameGraphResource DebugBuffer;
            };

            struct WaveIntrinsicsConstantBufferData
            {
                u32 waveMode = 1;
                u32 laneSize = 0;
            };

            framegraph.getBlackboard()
                .add<WaveIntrinsicsData>() = framegraph.addCallbackPass<WaveIntrinsicsData>(
                "[Test] Pass.Builtin.Code.WaveIntrinsics",
                [&](WaveIntrinsicsData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

#ifdef __APPLE__    // Metal cannot draw without a depth attachment
                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name      = "SceneDepth";
                    depthTextureDesc.width     = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height    = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format    = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.type      = TextureType::kDepth;
                    depthTextureDesc.initResourceViewHints = kDSV;
                    data.Depth                 = builder.create<FrameGraph::RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
#endif

                   //RZBufferDesc DebugDataBufferDesc{};
                   //DebugDataBufferDesc.name  = "WaveIntrinsicsConstantBufferData";
                   //DebugDataBufferDesc.size  = sizeof(SSAOParamsData);
                   //DebugDataBufferDesc.usage = BufferUsage::PersistentStream;
                   //data.DebugBuffer          = builder.create<FrameGraph::RZFrameGraphBuffer>(DebugDataBufferDesc.name, CAST_TO_FG_BUF_DESC DebugDataBufferDesc);
                   //
                   //data.DebugBuffer = builder.write(data.DebugBuffer);
                },
                [=](const WaveIntrinsicsData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("[Test] WaveIntrinsics Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.WaveIntrinsics", Utilities::GenerateHashedColor4(45u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
#ifdef __APPLE__    // Metal cannot draw without a depth attachment
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
#endif
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Bind the push CB with the render mode
                    // Set the Descriptor Set once rendering starts
                    //if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                    //    auto&         shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();
                    //    RZDescriptor* descriptor     = nullptr;
                    //    descriptor                   = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(data.DebugBuffer)];
                    //    if (descriptor)
                    //        descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.DebugBuffer).getHandle();
                    //    RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    //}
                    //
                    //WaveIntrinsicsConstantBufferData debugData{};
                    //debugData.waveMode = 2;
                    //debugData.laneSize = g_GraphicsFeatures.MinLaneWidth;
                    //
                    //auto DataHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.DebugBuffer).getHandle();
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

        void RZWaveInstrinsicsTestPass::destroy()
        {
            if (g_GraphicsFeatures.SupportsWaveIntrinsics)
                RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
