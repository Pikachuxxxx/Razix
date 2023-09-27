// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGIPass.h"

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

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Graphics/Renderers/RZCascadedShadowsRenderer.h"

namespace Razix {

    //-----------------------------------------------------------------------------------
    // RZGrid
    //-----------------------------------------------------------------------------------

    Maths::RZGrid::RZGrid(const Maths::AABB& _aabb)
        : aabb{_aabb}
    {
        const auto extent = aabb.getExtent();
        cellSize          = max3(extent) / kLPVResolution;
        size              = glm::uvec3{extent / cellSize + 0.5f};
    }

    namespace Graphics {

        void RZGIPass::addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Use this to get the Reflective shadow map cascade
            auto rsmLightViewProj = RZCascadedShadowsRenderer::buildCascades(scene->getSceneCamera(), glm::vec3(1.0f), 1, 1.0f, kRSMResolution)[0].viewProjMatrix;

            // RSM pass
            ReflectiveShadowMapData RSM = addRSMPass(framegraph, scene, rsmLightViewProj, glm::vec3(3.0f));

            // Add this to the blackboard
            framegraph.getBlackboard().add<ReflectiveShadowMapData>(RSM);

            // Radiance Injection Pass
            auto radiance = addRadianceInjectionPass(framegraph, RSM, m_Grid);

            LightPropagationVolumesData LPV{-1};

            // Create command buffers
            m_RadiancePropagationCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RadiancePropagationCmdBuffers[i] = RZCommandBuffer::Create();
                m_RadiancePropagationCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Radiance Propagation Command Buffers"));
            }

            for (u32 i = 0; i < kDefaultNumPropagations; i++)
                LPV = addRadiancePropagationPass(framegraph, i == 0 ? radiance : LPV, m_Grid, i);

            framegraph.getBlackboard().add<LightPropagationVolumesData>(LPV);
        }

        void RZGIPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD;
        }

        ReflectiveShadowMapData RZGIPass::addRSMPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, const glm::mat4& lightViewProj, glm::vec3 lightIntensity)
        {
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);

            // Create the command buffers
            m_RSMCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RSMCmdBuffers[i] = RZCommandBuffer::Create();
                m_RSMCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("RSM pass Command Buffers"));
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "RSM pipeline";
            pipelineInfo.cullMode            = Graphics::CullMode::None;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            // worldPos, normal, flux, Depth
            pipelineInfo.colorAttachmentFormats = {
                Graphics::TextureFormat::RGBA32F,
                Graphics::TextureFormat::RGBA32F,
                Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat = Graphics::TextureFormat::DEPTH32F;

            m_RSMPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameblockData = framegraph.getBlackboard().get<FrameData>();

            auto& data = framegraph.addCallbackPass<ReflectiveShadowMapData>(
                "Reflective Shadow Map",
                [&](ReflectiveShadowMapData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{
                        .name   = "RSM/Position",
                        .width  = kRSMResolution,
                        .height = kRSMResolution,
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    // Create the output RTs
                    data.position = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Position", CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name = "RSM/Normal";

                    data.normal = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Normal", CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name = "RSM/Flux";

                    data.flux = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Flux", CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name      = "RSM/Depth";
                    textureDesc.format    = TextureFormat::DEPTH32F;
                    textureDesc.filtering = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    textureDesc.type      = TextureType::Texture_Depth;

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Depth", CAST_TO_FG_TEX_DESC textureDesc);

                    data.position = builder.write(data.position);
                    data.normal   = builder.write(data.normal);
                    data.flux     = builder.write(data.flux);
                    data.depth    = builder.write(data.depth);

                    builder.read(frameblockData.frameData);
                },
                [=](const ReflectiveShadowMapData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_RSMCmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "RSM Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("ReflectiveShadowMap", glm::vec4(.23f, .45f, .76f, 1.0f))

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.position).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 0
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.normal).getHandle(), {true, ClearColorPresets::TransparentBlack}},      // location = 1
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.flux).getHandle(), {true, ClearColorPresets::TransparentBlack}},        // location = 2

                    };
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true}};
                    info.extent          = {kRSMResolution, kRSMResolution};
                    info.resize          = false;

                    RHI::BeginRendering(cmdBuffer, info);

#if 1
                    // Bind the pipeline
                    RHI::BindPipeline(m_RSMPipeline, cmdBuffer);

                    // Update the View Projection descriptor set only once
                    static bool setUpdated = false;
                    if (!setUpdated) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            if (setInfo.first == BindingTable_System::SET_IDX_SYSTEM_START) {
                                for (auto& descriptor: setInfo.second) {
                                    if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                                        descriptor.uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameblockData.frameData).getHandle();
                                        m_MVPDescriptorSet       = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("MVP GI Pass Set"));
                                    }
                                }
                            }
                        }
                        setUpdated = true;
                    }

                    // TODO: Use scene to draw the geometry

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = RZResourceManager::Get().getShaderResource(shader)->getPushConstants()[0];

                        struct PCD
                        {
                            glm::mat4 mat;
                        } pcData{};
                        pcData.mat       = transform;
                        modelMatrix.data = &pcData;
                        modelMatrix.size = sizeof(PCD);

                        // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                        Graphics::RHI::BindPushConstant(m_RSMPipeline, cmdBuffer, modelMatrix);
                        //-----------------------------

                        // Combine System Desc sets with material sets and Bind them
                        std::vector<RZDescriptorSet*> SystemMat = {m_MVPDescriptorSet, mrc.Mesh->getMaterial()->getDescriptorSet()};

                        Graphics::RHI::BindUserDescriptorSets(m_RSMPipeline, cmdBuffer, SystemMat);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                        Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }
                // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
#endif

                    RAZIX_MARK_END();
                    RHI::EndRendering(cmdBuffer);

                    RHI::Submit(cmdBuffer);
                    RHI::SubmitWork({}, {});
                });

            return data;
        }

        LightPropagationVolumesData RZGIPass::addRadianceInjectionPass(FrameGraph::RZFrameGraph& framegraph, const ReflectiveShadowMapData& RSM, const Maths::RZGrid& grid)
        {
            // FIXME!!!: invert the RT as vulkan need inverted Y

            // Load the shader properly
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);

            // Get the setinfo ==> allocate UBOs and Textures and bind them to the descriptor sets
            m_RadianceInjectionUBO = RZResourceManager::Get().createUniformBuffer({"RI UBO", sizeof(RadianceInjectionUBOData), nullptr});

            // Create command buffers
            m_RadianceInjectionCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RadianceInjectionCmdBuffers[i] = RZCommandBuffer::Create();
                m_RadianceInjectionCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Radiance Injection Command Buffers"));
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Radiance Injection pipeline";
            pipelineInfo.cullMode            = Graphics::CullMode::None;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::Point;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = false;
            pipelineInfo.depthWriteEnabled   = false;
            // Additive Blending
            pipelineInfo.colorSrc = BlendFactor::One;
            pipelineInfo.colorDst = BlendFactor::One;
            pipelineInfo.alphaSrc = BlendFactor::One;
            pipelineInfo.alphaDst = BlendFactor::One;
            // Depth, worldPos, normal, flux
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F, Graphics::TextureFormat::RGBA32F, Graphics::TextureFormat::RGBA32F};

            m_RIPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Frame Graph pass

            const LightPropagationVolumesData data = framegraph.addCallbackPass<LightPropagationVolumesData>(
                "Radiance Injection",
                [&](LightPropagationVolumesData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(RSM.position);
                    builder.read(RSM.normal);
                    builder.read(RSM.flux);

                    RZTextureDesc textureDesc{
                        .name   = "SH/R",
                        .width  = grid.size.x,
                        .height = grid.size.y,
                        .layers = grid.size.z,
                        .type   = TextureType::Texture_3D,
                        .format = TextureFormat::RGBA32F};

                    // Create the resource for this pass
                    data.r           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/R", CAST_TO_FG_TEX_DESC textureDesc);
                    textureDesc.name = "SH/G";
                    data.g           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/G", CAST_TO_FG_TEX_DESC textureDesc);
                    textureDesc.name = "SH/B";
                    data.b           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/B", CAST_TO_FG_TEX_DESC textureDesc);

                    data.r = builder.write(data.r);
                    data.g = builder.write(data.g);
                    data.b = builder.write(data.b);
                },
                [=](const LightPropagationVolumesData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

#if 1
                    auto cmdBuffer = m_RadianceInjectionCmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "Radiance Injection Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("Radiance Injection", glm::vec4(.53f, .45f, .76f, 1.0f))

    #if 1
                    static bool setsCreated = false;
                    if (!setsCreated) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                                    descriptor.uniformBuffer = m_RadianceInjectionUBO;
                                } else {
                                    switch (descriptor.bindingInfo.location.binding) {
                                        case 1:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(RSM.position).getHandle();
                                            break;
                                        case 2:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(RSM.normal).getHandle();
                                            break;
                                        case 3:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(RSM.flux).getHandle();
                                            break;
                                    }
                                }
                            }
                            m_RIDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Radiance Injection Pass Set"));
                        }
                        setsCreated = true;
                    }
    #endif

                    RenderingInfo info{};
                    info.extent           = {grid.size.x, grid.size.y};
                    info.layerCount       = 1;    //grid.size.z; // Since we are using 3D texture they only have a single layer
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.r).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 0 // SH_R
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.g).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 1 // SH_G
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.b).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 2 // SH_B
                    };

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind the sets and update the data
                    radianceInjectionData.RSMResolution = kRSMResolution;
                    radianceInjectionData.MinCorner     = grid.aabb.min;
                    radianceInjectionData.GridSize      = grid.size;
                    radianceInjectionData.CellSize      = grid.cellSize;

                    auto buffer = RZResourceManager::Get().getUniformBufferResource(m_RadianceInjectionUBO);
                    buffer->SetData(sizeof(RadianceInjectionUBOData), &radianceInjectionData);

                    // Bind the pipeline
                    RHI::BindPipeline(m_RIPipeline, cmdBuffer);

                    // Bind the desc sets
                    //RHI::BindUserDescriptorSets(m_RIPipeline, cmdBuffer, &m_RIDescriptorSet, 1);
                    RHI::BindDescriptorSet(m_RIPipeline, cmdBuffer, m_RIDescriptorSet, 0);

                    RHI::Draw(cmdBuffer, kNumVPL);

                    RAZIX_MARK_END();
                    RHI::EndRendering(cmdBuffer);

                    RHI::Submit(cmdBuffer);
                    RHI::SubmitWork({}, {});
#endif
                });
            return data;
        }

        LightPropagationVolumesData RZGIPass::addRadiancePropagationPass(FrameGraph::RZFrameGraph& framegraph, const LightPropagationVolumesData& LPV, const Maths::RZGrid& grid, u32 propagationIdx)
        {
            // First order of business get the shader
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            // Get the setinfo ==> allocate UBOs and Textures and bind them to the descriptor sets
            m_RadiancePropagationUBO = RZResourceManager::Get().createUniformBuffer({"RPropagation UBO", sizeof(RadiancePropagationUBOData), nullptr});

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Radiance Propagation pipeline";
            pipelineInfo.cullMode            = Graphics::CullMode::None;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::Point;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = false;
            pipelineInfo.depthWriteEnabled   = false;
            // Additive Blending
            pipelineInfo.colorSrc = BlendFactor::One;
            pipelineInfo.colorDst = BlendFactor::One;
            pipelineInfo.alphaSrc = BlendFactor::One;
            pipelineInfo.alphaDst = BlendFactor::One;
            // Depth, worldPos, normal, flux
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F, Graphics::TextureFormat::RGBA32F, Graphics::TextureFormat::RGBA32F};

            m_RPropagationPipeline = RZResourceManager::Get().createPipeline((pipelineInfo));

            const auto& data = framegraph.addCallbackPass<LightPropagationVolumesData>(
                "Radiance Propagation #" + std::to_string(propagationIdx),
                [&](LightPropagationVolumesData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(LPV.r);
                    builder.read(LPV.g);
                    builder.read(LPV.b);

                    RZTextureDesc textureDesc{
                        .name   = "SH/R",
                        .width  = grid.size.x,
                        .height = grid.size.y,
                        .layers = grid.size.z,
                        .type   = TextureType::Texture_3D,
                        .format = TextureFormat::RGBA32F};

                    // Create the resource for this pass
                    data.r           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/R", CAST_TO_FG_TEX_DESC textureDesc);
                    textureDesc.name = "SH/G";
                    data.g           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/G", CAST_TO_FG_TEX_DESC textureDesc);
                    textureDesc.name = "SH/B";
                    data.b           = builder.create<FrameGraph::RZFrameGraphTexture>("SH/B", CAST_TO_FG_TEX_DESC textureDesc);

                    data.r = builder.write(data.r);
                    data.g = builder.write(data.g);
                    data.b = builder.write(data.b);
                },
                [=](const LightPropagationVolumesData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_RadiancePropagationCmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "Radiance Propagation Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("Radiance Propagation", glm::vec4(.53f, .45f, .16f, 1.0f))

                    if (!m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                                    descriptor.uniformBuffer = m_RadiancePropagationUBO;
                                } else {
                                    switch (descriptor.bindingInfo.location.binding) {
                                        case 1:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV.r).getHandle();
                                            break;
                                        case 2:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV.g).getHandle();
                                            break;
                                        case 3:
                                            descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(LPV.b).getHandle();
                                            break;
                                    }
                                }
                            }
                            m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Radiance Propagation Pass Set #" + std::to_string(propagationIdx)));
                        }
                    }

                    RenderingInfo info{};
                    info.extent           = {grid.size.x, grid.size.y};
                    info.layerCount       = 1;    //grid.size.z; // Since we are using 3D texture they only have a single layer
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.r).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 0 // SH_R
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.g).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 1 // SH_G
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.b).getHandle(), {true, ClearColorPresets::TransparentBlack}},    // location = 2 // SH_B
                    };

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind the sets and update the data
                    radiancePropagationData.GridSize = grid.size;

                    auto buffer = RZResourceManager::Get().getUniformBufferResource(m_RadiancePropagationUBO);
                    buffer->SetData(sizeof(RadianceInjectionUBOData), &radiancePropagationData);

                    // Bind the pipeline
                    RHI::BindPipeline(m_RPropagationPipeline, cmdBuffer);

                    // Bind the desc sets
                    //RHI::BindUserDescriptorSets(m_RPropagationPipeline, cmdBuffer, &m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet, 1);
                    RHI::BindDescriptorSet(m_RPropagationPipeline, cmdBuffer, m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet, 0);

                    RHI::Draw(cmdBuffer, kNumVPL);

                    RAZIX_MARK_END();
                    RHI::EndRendering(cmdBuffer);

                    RHI::Submit(cmdBuffer);
                    RHI::SubmitWork({}, {});
                });
            return data;
        }
    }    // namespace Graphics
}    // namespace Razix