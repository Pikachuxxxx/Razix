// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDeferredLightingPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
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

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/GBufferData.h"
#include "Razix/Graphics/Passes/Data/GIData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"
#include "Razix/Graphics/Passes/Data/ShadowMapData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZDeferredLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Get data from the blackboard
            // Get the GBuffer data
            const GBufferData& gBuffer      = framegraph.getBlackboard().get<GBufferData>();
            auto               gbuffer_desc = framegraph.getDescriptor<FrameGraph::RZFrameGraphTexture>(gBuffer.Depth);
            const auto         extent       = glm::vec2(gbuffer_desc.width, gbuffer_desc.height);

            // BRDF
            const BRDFData& brdf = framegraph.getBlackboard().get<BRDFData>();

            // Light probe data
            const auto& globalLightProbe = framegraph.getBlackboard().get<GlobalLightProbeData>();

            // Cascade shadow maps
            const ShadowMapData& cascades = framegraph.getBlackboard().get<ShadowMapData>();

            // GI data
            const LightPropagationVolumesData* LPV = framegraph.getBlackboard().try_get<LightPropagationVolumesData>();

            // Shader, UBOs & Sets, Pipeline, CmdBuffers
            // FrameBlock UBO
            // TODO: Import this into the framegraph Blackboard and make it available globally to all passes
            //m_FrameBlockUBO = RZUniformBuffer::Create(sizeof(FrameBlock), &m_FrameBlockData RZ_DEBUG_NAME_TAG_STR_E_ARG("FrameBlock UBO"));

            // Tile Data UBO
            m_TileData.MinCorner = m_Grid.aabb.min;
            m_TileData.GridSize  = m_Grid.size;
            m_TileData.CellSize  = m_Grid.cellSize;
            m_TileDataUBO        = RZResourceManager::Get().createUniformBuffer({"Tile Data UBO ", sizeof(TileData), &m_TileData});

            // Lights UBO
            m_LightDataUBO = RZResourceManager::Get().createUniformBuffer({"Light Data UBO", sizeof(GPULightsData), nullptr});

            auto shader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();

            RZPipelineDesc info{};
            info.name                   = "Deferred Lighting Pipeline";
            info.shader                 = shader;
            info.colorAttachmentFormats = {TextureFormat::RGBA32F};

            m_Pipeline = RZResourceManager::Get().createPipeline(info);

            for (sz i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Deferred Lighting CmdBufs"));
            }

            // Screen Quad Mesh
            m_ScreenQuadMesh = MeshFactory::CreatePrimitive(MeshPrimitive::ScreenQuad);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();

            framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Deferred PBR Lighting Pass",
                [&](SceneData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    // TEMP:
                    builder.setAsStandAlonePass();

                    // Reads
                    /*      builder.read(gBuffer.Normal_PosX);
                    builder.read(gBuffer.Albedo_PosY);
                    builder.read(gBuffer.Emissive_PosZ);
                    builder.read(gBuffer.MetRougAOAlpha);*/
                    builder.read(gBuffer.Depth);

                    builder.read(brdf.lut);

                    builder.read(globalLightProbe.diffuseIrradianceMap);
                    builder.read(globalLightProbe.specularPreFilteredMap);

                    builder.read(frameDataBlock.frameData);

                    //builder.read(cascades.cascadedShadowMaps);
                    //builder.read(cascades.viewProjMatrices);

                    //if (LPV) {
                    //    builder.read(LPV->r);
                    //    builder.read(LPV->g);
                    //    builder.read(LPV->b);
                    //}

                    // Write to a HDR render target
                    RZTextureDesc sceneHDRDesc{
                        .name   = "Scene HDR Color",
                        .width  = static_cast<u32>(extent.x),
                        .height = static_cast<u32>(extent.y),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR color", CAST_TO_FG_TEX_DESC sceneHDRDesc);

                    data.outputHDR = builder.write(data.outputHDR);
                },
                [=](const SceneData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuf = m_CmdBuffers[Graphics::RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Deferred Tiled Pass", glm::vec4(0.2, 0.4, 0.6, 1.0f));

                    glm::vec2 resolution = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent = resolution;
                    info.resize = true;

                    RHI::BeginRendering(cmdBuf, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuf);

                    //m_FrameBlockUBO->SetData(sizeof(FrameBlock), &m_FrameBlockData);

                    // Update the lighting Data
                    GPULightsData m_GPULightData{};
                    //auto          group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
                    //for (auto entity: group) {
                    //    const auto& [light, trans] = group.get<LightComponent, TransformComponent>(entity);
                    //    m_GPULightData.numLights++;
                    //    m_GPULightData.data = light.light.getLightData();
                    //}

                    RZResourceManager::Get().getUniformBufferResource(m_LightDataUBO)->SetData(sizeof(GPULightsData), &m_GPULightData);

                // Update the Sets only once on first frame to get runtime framegraph resources
#if 0
                    static bool setsUpdated   = false;
                    static bool didCreateOnce = false;
                    if (!setsUpdated) {
                        for (auto set: setInfos) {
                            // FrameBlock UBO [SET: 0]
                            if (set.first == 0) {
                                for (auto& descriptor: set.second)
                                    descriptor.uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();
                            }
                            // [SET:1]
                            else if (set.first == 1) {
                                for (auto& descriptor: set.second) {
                                    // This needs to be updated only once when the framegraph resources are available in the execute lambda
                                    switch (descriptor.bindingInfo.location.binding) {
                                        case 0:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Normal_PosX).getHandle();
                                            break;
                                        case 1:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Normal_PosX).getHandle();
                                            break;
                                        case 2:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Albedo_PosY).getHandle();
                                            break;
                                        case 3:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Emissive_PosZ).getHandle();
                                            break;
                                        case 4:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.MetRougAOAlpha).getHandle();
                                            break;
                                        case 5:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(brdf.lut).getHandle();
                                            break;
                                        case 6:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbe.diffuseIrradianceMap).getHandle();
                                            break;
                                        case 7:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbe.specularPreFilteredMap).getHandle();
                                            break;
                                            /*case 8:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(cascades.cascadedShadowMaps).getHandle();
                                            break;
                                        case 9:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->r).getHandle();
                                            break;
                                        case 10:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->g).getHandle();
                                            break;
                                        case 11:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->b).getHandle();
                                            break;
                                        case 12:
                                            descriptor.uniformBuffer = m_TileDataUBO;
                                            break;*/
                                    }
                                }
                            }
                            // [SET:2]
                            else if (set.first == 2) {
                                for (auto& descriptor: set.second) {
                                    switch (descriptor.bindingInfo.location.binding) {
                                        case 0:
                                            descriptor.uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(cascades.viewProjMatrices).getHandle();
                                            break;
                                        case 1:
                                            descriptor.uniformBuffer = m_LightDataUBO;
                                            break;
                                    }
                                }
                            }
                            m_DescriptorSets.push_back(RZDescriptorSet::Create(set.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Deferred Tiled Set")));
                        }
                        setsUpdated = true;
                    }

                    // Update the second set
                    for (auto set: setInfos) {
                        if (set.first == 1) {
                            for (auto& descriptor: set.second) {
                                // This needs to be updated only once when the framegraph resources are available in the execute lambda
                                switch (descriptor.bindingInfo.location.binding) {
                                    case 0:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Normal).getHandle();
                                        break;
                                    case 1:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Normal).getHandle();
                                        break;
                                    case 2:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Albedo).getHandle();
                                        break;
                                    case 3:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.Emissive).getHandle();
                                        break;
                                    case 4:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(gBuffer.MetRougAOSpec).getHandle();
                                        break;
                                    case 5:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(brdf.lut).getHandle();
                                        break;
                                    case 6:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbe.diffuseIrradianceMap).getHandle();
                                        break;
                                    case 7:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbe.specularPreFilteredMap).getHandle();
                                        break;
                                        /*            case 8:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(cascades.cascadedShadowMaps).getHandle();
                                        break;
                                    case 9:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->r).getHandle();
                                        break;
                                    case 10:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->g).getHandle();
                                        break;
                                    case 11:
                                        descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV->b).getHandle();
                                        break;
                                    case 12:
                                        descriptor.uniformBuffer = m_TileDataUBO;
                                        break;*/
                                }
                            }
                            m_DescriptorSets[1]->UpdateSet(set.second);
                        }
                    }
#endif

                    // Bind the descriptor sets
                    Graphics::RHI::BindUserDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

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