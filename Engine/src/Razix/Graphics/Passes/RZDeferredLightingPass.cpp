// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDeferredLightingPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/GBufferData.h"
#include "Razix/Graphics/Passes/Data/GIData.h"
#include "Razix/Graphics/Passes/Data/GlobalLightProbeData.h"
#include "Razix/Graphics/Passes/Data/ShadowMapData.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {

        void RZDeferredLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Get data from the blackboard
            // Get the GBuffer data
            const GBufferData& gBuffer = blackboard.get<GBufferData>();
            const auto         extent  = framegraph.getDescriptor<FrameGraph::RZFrameGraphTexture>(gBuffer.Depth).extent;

            // BRDF
            const BRDFData& brdf = blackboard.get<BRDFData>();

            // Light probe data
            const auto& globalLightProbe = blackboard.get<GlobalLightProbeData>();

            // Cascade shadow maps
            const ShadowMapData& cascades = blackboard.get<ShadowMapData>();

            // GI data
            const LightPropagationVolumesData* LPV = blackboard.try_get<LightPropagationVolumesData>();

            // Shader, UBOs & Sets, Pipeline, CmdBuffers

            auto shader   = RZShaderLibrary::Get().getShader("DeferredTiledLighting.rzsf");
            auto setInfos = shader->getSetsCreateInfos();

            PipelineInfo info{};
            info.shader                 = shader;
            info.colorAttachmentFormats = {RZTexture::Format::RGBA32F};

            m_Pipeline = RZPipeline::Create(info RZ_DEBUG_NAME_TAG_STR_E_ARG("Deferred Lighting Pipeline"));

            for (size_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Deferred Lighting CmdBufs"));
            }

            framegraph.addCallbackPass<SceneColorData>(
                "Deferred PBR Lighting Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneColorData& data) {
                    // Reads
                    builder.read(gBuffer.Albedo);
                    builder.read(gBuffer.Depth);
                    builder.read(gBuffer.Emissive);
                    builder.read(gBuffer.MetRougAOSpec);
                    builder.read(gBuffer.Normal);

                    builder.read(brdf.lut);

                    builder.read(globalLightProbe.diffuseIrradianceMap);
                    builder.read(globalLightProbe.specularPreFilteredMap);

                    builder.read(cascades.cascadedShadowMaps);
                    builder.read(cascades.viewProjMatrices);

                    if (LPV) {
                        builder.read(LPV->r);
                        builder.read(LPV->g);
                        builder.read(LPV->b);
                    }

                    // Write to a HDR render target
                    data.HDR = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR color", {FrameGraph::TextureType::Texture_RenderTarget, "Scene HDR color", {extent.x, extent.y}, RZTexture::Format::RGBA32F});

                    data.HDR = builder.write(data.HDR);
                },
                [=](const SceneColorData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {

                });
        }

        void RZDeferredLightingPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix