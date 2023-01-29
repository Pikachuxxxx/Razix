// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDeferredLightingPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/Lighting/RZLight.h"

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
            // FrameBlock UBO
            // TODO: Import this into the framegraph Blackboard and make it available globally to all passes
            m_FrameBlockUBO = RZUniformBuffer::Create(sizeof(FrameBlock), &m_FrameBlockData RZ_DEBUG_NAME_TAG_STR_E_ARG("FrameBlock UBO"));

            // Tile Data UBO
            m_TileData.MinCorner = m_Grid.aabb.min;
            m_TileData.GridSize  = m_Grid.size;
            m_TileData.CellSize  = m_Grid.cellSize;
            m_TileDataUBO        = RZUniformBuffer::Create(sizeof(TileData), &m_TileData RZ_DEBUG_NAME_TAG_STR_E_ARG("Tile Data UBO"));

            // Lights UBO
            m_LightDataUBO = RZUniformBuffer::Create(sizeof(LightData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("Light Data UBO"));

            auto  shader   = RZShaderLibrary::Get().getShader("DeferredTiledLighting.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();

            for (auto& set: setInfos) {
                // FrameBlock UBO [SET: 0]
                if (set.first == 0) {
                    for (auto& descriptor: set.second)
                        descriptor.uniformBuffer = m_FrameBlockUBO;
                }
                // [SET:1]
                else if (set.first == 1) {
                    for (auto& descriptor: set.second) {
                        // This needs to be updated only once when the framegraph resources are available in the execute lambda
                        switch (descriptor.bindingInfo.binding) {
                            case 0:
                                descriptor.texture = framegraph.getResourceEntry(gBuffer.Depth).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 1:
                                descriptor.texture = framegraph.getResourceEntry(gBuffer.Normal).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 2:
                                descriptor.texture = framegraph.getResourceEntry(gBuffer.Albedo).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 3:
                                descriptor.texture = framegraph.getResourceEntry(gBuffer.Emissive).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 4:
                                descriptor.texture = framegraph.getResourceEntry(gBuffer.MetRougAOSpec).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 5:
                                descriptor.texture = framegraph.getResourceEntry(brdf.lut).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 6:
                                descriptor.texture = framegraph.getResourceEntry(globalLightProbe.diffuseIrradianceMap).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 7:
                                descriptor.texture = framegraph.getResourceEntry(globalLightProbe.specularPreFilteredMap).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 8:
                                descriptor.texture = framegraph.getResourceEntry(cascades.cascadedShadowMaps).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 9:
                                descriptor.texture = framegraph.getResourceEntry(LPV->r).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 10:
                                descriptor.texture = framegraph.getResourceEntry(LPV->g).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 11:
                                descriptor.texture = framegraph.getResourceEntry(LPV->b).get<FrameGraph::RZFrameGraphTexture>().getHandle();
                                break;
                            case 12:
                                descriptor.uniformBuffer = m_TileDataUBO;
                                break;
                        }
                    }

                }
                // [SET:2]
                else if (set.first == 2) {
                    for (auto& descriptor: set.second) {
                        switch (descriptor.bindingInfo.binding) {
                            case 0:
                                descriptor.uniformBuffer = framegraph.getResourceEntry(cascades.viewProjMatrices).get<FrameGraph::RZFrameGraphBuffer>().getHandle();
                                break;
                            case 1:
                                descriptor.uniformBuffer = m_LightDataUBO;
                                break;
                        }
                    }
                }
            }

            PipelineInfo info{};
            info.shader                 = shader;
            info.colorAttachmentFormats = {RZTexture::Format::RGBA32F};

            m_Pipeline = RZPipeline::Create(info RZ_DEBUG_NAME_TAG_STR_E_ARG("Deferred Lighting Pipeline"));

            for (size_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Deferred Lighting CmdBufs"));
            }

            // Screen Quad Mesh
            m_ScreenQuadMesh = MeshFactory::CreatePrimitive(MeshPrimitive::ScreenQuad);

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
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuf = m_CmdBuffers[Graphics::RHI::getSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Deferred Tiled Pass", glm::vec4(0.5f));

                    cmdBuf->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.HDR).getHandle(), {true}}};
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuf, info);

                    // Bind pipeline and stuff
                    m_Pipeline->Bind(cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

                    // Draw the Mesh
                    m_ScreenQuadMesh->getVertexBuffer()->Bind(cmdBuf);
                    m_ScreenQuadMesh->getIndexBuffer()->Bind(cmdBuf);

                    RHI::DrawIndexed(cmdBuf, m_ScreenQuadMesh->getIndexCount());

                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RHI::Submit(cmdBuf);

                    RHI::SubmitWork({}, {});
                });
        }

        void RZDeferredLightingPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix