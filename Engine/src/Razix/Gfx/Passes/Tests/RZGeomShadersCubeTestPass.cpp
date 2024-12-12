// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGeomShadersCubeTestPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Passes/Data/FrameData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

#define NUM_CUBE_FACES 6

        void RZGeomShadersCubeTestPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GSCubeTest);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.GSCube";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
            pipelineInfo.depthFormat            = TextureFormat::DEPTH16_UNORM;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Point;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            struct GSCubeData
            {
                FrameGraph::RZFrameGraphResource Depth;
            };

            auto& frameData = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<GSCubeData>() = framegraph.addCallbackPass<GSCubeData>(
                "[Test] Pass.Builtin.Code.GSCube",
                [&](GSCubeData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name      = "SceneDepth";
                    depthTextureDesc.width     = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height    = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format    = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.filtering = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    depthTextureDesc.type      = TextureType::Texture_Depth;
                    data.Depth                 = builder.create<FrameGraph::RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);
                },
                [=](const GSCubeData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("[Test] GS Cube Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.GSCube", Utilities::GenerateHashedColor4(69u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    info.resize           = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // bound to GS
                    Gfx::RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, Gfx::RHI::Get().getFrameDataSet(), BindingTable_System::BINDING_IDX_FRAME_DATA);

                    // We want to a single draw-call since, using a cube mesh will need a single draw-call we keep it to that constrain
                    // We invoke a point per face and turn that into a face
                    Gfx::RHI::Draw(cmdBuffer, NUM_CUBE_FACES);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZGeomShadersCubeTestPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
