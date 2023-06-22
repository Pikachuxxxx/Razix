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

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Graphics/Renderers/RZCascadedShadowsRenderer.h"

namespace Razix {

    //-----------------------------------------------------------------------------------
    // RZGrid
    //-----------------------------------------------------------------------------------

    Maths::RZGrid::RZGrid(const Maths::RZAABB& _aabb)
        : aabb{_aabb}
    {
        const auto extent = aabb.getExtent();
        cellSize          = max3(extent) / kLPVResolution;
        size              = glm::uvec3{extent / cellSize + 0.5f};
    }

    namespace Graphics {

        void RZGIPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Use this to get the Reflective shadow map cascade
            auto rsmLightViewProj = RZCascadedShadowsRenderer::buildCascades(scene->getSceneCamera(), glm::vec3(1.0f), 1, 1.0f, kRSMResolution)[0].viewProjMatrix;

            // RSM pass
            ReflectiveShadowMapData RSM = addRSMPass(framegraph, blackboard, scene, rsmLightViewProj, glm::vec3(3.0f));

            // Add this to the blackboard
            blackboard.add<ReflectiveShadowMapData>(RSM);

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

            blackboard.add<LightPropagationVolumesData>(LPV);
        }

        void RZGIPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD;
        }

        ReflectiveShadowMapData RZGIPass::addRSMPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, const glm::mat4& lightViewProj, glm::vec3 lightIntensity)
        {
            auto shader = RZShaderLibrary::Get().getShader("RSM.rzsf");

            // Create the command buffers
            m_RSMCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RSMCmdBuffers[i] = RZCommandBuffer::Create();
                m_RSMCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("RSM pass Command Buffers"));
            }

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            // worldPos, normal, flux, Depth
            pipelineInfo.colorAttachmentFormats = {
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F};
            pipelineInfo.depthFormat = Graphics::RZTextureProperties::Format::DEPTH16_UNORM;

            m_RSMPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("RSM pipeline"));

            auto& frameblockData = blackboard.get<FrameData>();

            auto& data = framegraph.addCallbackPass<ReflectiveShadowMapData>(
                "Reflective Shadow Map",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, ReflectiveShadowMapData& data) {
                    builder.setAsStandAlonePass();

                    // Create the output RTs
                    data.position = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Position", {FrameGraph::RZTextureProperties::Type::Texture_RenderTarget, "RSM/Position", {kRSMResolution, kRSMResolution}, RZTextureProperties::Format::RGBA32F});

                    data.normal = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Normal", {FrameGraph::RZTextureProperties::Type::Texture_RenderTarget, "RSM/Normal", {kRSMResolution, kRSMResolution}, RZTextureProperties::Format::RGBA32F});

                    data.flux = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Flux", {FrameGraph::RZTextureProperties::Type::Texture_RenderTarget, "RSM/Flux", {kRSMResolution, kRSMResolution}, RZTextureProperties::Format::RGBA32F});

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Depth", {FrameGraph::RZTextureProperties::Type::Texture_Depth, "RSM/Depth", {kRSMResolution, kRSMResolution}, RZTextureProperties::Format::DEPTH16_UNORM});

                    data.position = builder.write(data.position);
                    data.normal   = builder.write(data.normal);
                    data.flux     = builder.write(data.flux);
                    data.depth    = builder.write(data.depth);

                    builder.read(frameblockData.frameData);
                },
                [=](const ReflectiveShadowMapData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_RSMCmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "RSM Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("ReflectiveShadowMap", glm::vec4(.23f, .45f, .76f, 1.0f))

                    cmdBuffer->UpdateViewport(kRSMResolution, kRSMResolution);

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.position).getHandle(), {true, glm::vec4(0.0f)}},    // location = 0
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.normal).getHandle(), {true, glm::vec4(0.0f)}},      // location = 1
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.flux).getHandle(), {true, glm::vec4(0.0f)}},        // location = 2

                    };
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true}};
                    info.extent          = {kRSMResolution, kRSMResolution};
                    info.resize          = false;

                    RHI::BeginRendering(cmdBuffer, info);

#if 1
                    // Bind the pipeline
                    m_RSMPipeline->Bind(cmdBuffer);

                    // Update the View Projection descriptor set only once
                    static bool setUpdated = false;
                    if (!setUpdated) {
                        auto& setInfos = shader->getSetsCreateInfos();
                        for (auto& setInfo: setInfos) {
                            if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_VIEW_PROJECTION) {
                                for (auto& descriptor: setInfo.second) {
                                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
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
                    auto& mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = shader->getPushConstants()[0];

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

                        Graphics::RHI::BindDescriptorSets(m_RSMPipeline, cmdBuffer, SystemMat);

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
            auto shader = RZShaderLibrary::Get().getShader("lpv_radiance_injection.rzsf");

            // Get the setinfo ==> allocate UBOs and Textures and bind them to the descriptor sets
            m_RadianceInjectionUBO = RZUniformBuffer::Create(sizeof(RadianceInjectionUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("RI UBO"));

            // Create command buffers
            m_RadianceInjectionCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RadianceInjectionCmdBuffers[i] = RZCommandBuffer::Create();
                m_RadianceInjectionCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Radiance Injection Command Buffers"));
            }

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::POINT;
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
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F, Graphics::RZTextureProperties::Format::RGBA32F, Graphics::RZTextureProperties::Format::RGBA32F};

            m_RIPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Radiance Injection pipeline"));

            // Frame Graph pass

            const LightPropagationVolumesData data = framegraph.addCallbackPass<LightPropagationVolumesData>(
                "Radiance Injection",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, LightPropagationVolumesData& data) {
                    builder.setAsStandAlonePass();

                    builder.read(RSM.position);
                    builder.read(RSM.normal);
                    builder.read(RSM.flux);

                    // Create the resource for this pass
                    data.r = builder.create<FrameGraph::RZFrameGraphTexture>("SH/R", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/R", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});
                    data.g = builder.create<FrameGraph::RZFrameGraphTexture>("SH/G", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/G", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});
                    data.b = builder.create<FrameGraph::RZFrameGraphTexture>("SH/B", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/B", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});

                    data.r = builder.write(data.r);
                    data.g = builder.write(data.g);
                    data.b = builder.write(data.b);
                },
                [=](const LightPropagationVolumesData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
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

                    cmdBuffer->UpdateViewport(grid.size.x, grid.size.y);

    #if 1
                    static bool setsCreated = false;
                    if (!setsCreated) {
                        auto setInfos = shader->getSetsCreateInfos();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                                    descriptor.uniformBuffer = m_RadianceInjectionUBO;
                                } else {
                                    switch (descriptor.bindingInfo.binding) {
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
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.r).getHandle(), {true, glm::vec4(0.0f)}},    // location = 0 // SH_R
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.g).getHandle(), {true, glm::vec4(0.0f)}},    // location = 1 // SH_G
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.b).getHandle(), {true, glm::vec4(0.0f)}},    // location = 2 // SH_B
                    };

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind the sets and update the data
                    radianceInjectionData.RSMResolution = kRSMResolution;
                    radianceInjectionData.MinCorner     = grid.aabb.min;
                    radianceInjectionData.GridSize      = grid.size;
                    radianceInjectionData.CellSize      = grid.cellSize;

                    m_RadianceInjectionUBO->SetData(sizeof(RadianceInjectionUBOData), &radianceInjectionData);

                    // Bind the pipeline
                    m_RIPipeline->Bind(cmdBuffer);

                    // Bind the desc sets
                    RHI::BindDescriptorSets(m_RIPipeline, cmdBuffer, &m_RIDescriptorSet, 1);

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
            auto shader = RZShaderLibrary::Get().getShader("lpv_radiance_propagation.rzsf");
            // Get the setinfo ==> allocate UBOs and Textures and bind them to the descriptor sets
            m_RadiancePropagationUBO = RZUniformBuffer::Create(sizeof(RadiancePropagationUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("RPropagation UBO"));

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::POINT;
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
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F, Graphics::RZTextureProperties::Format::RGBA32F, Graphics::RZTextureProperties::Format::RGBA32F};

            m_RPropagationPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Radiance Propagation pipeline"));

            const auto& data = framegraph.addCallbackPass<LightPropagationVolumesData>(
                "Radiance Propagation #" + std::to_string(propagationIdx),
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, LightPropagationVolumesData& data) {
                    builder.setAsStandAlonePass();

                    builder.read(LPV.r);
                    builder.read(LPV.g);
                    builder.read(LPV.b);

                    // Create the resource for this pass
                    data.r = builder.create<FrameGraph::RZFrameGraphTexture>("SH/R", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/R", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});
                    data.g = builder.create<FrameGraph::RZFrameGraphTexture>("SH/G", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/G", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});
                    data.b = builder.create<FrameGraph::RZFrameGraphTexture>("SH/B", {FrameGraph::RZTextureProperties::Type::Texture_3D, "SH/B", {grid.size.x, grid.size.y}, RZTextureProperties::Format::RGBA32F, grid.size.z});

                    data.r = builder.write(data.r);
                    data.g = builder.write(data.g);
                    data.b = builder.write(data.b);
                },
                [=](const LightPropagationVolumesData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_RadiancePropagationCmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "Radiance Propagation Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("Radiance Propagation", glm::vec4(.53f, .45f, .16f, 1.0f))

                    cmdBuffer->UpdateViewport(grid.size.x, grid.size.y);

                    if (!m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet) {
                        auto setInfos = shader->getSetsCreateInfos();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                                    descriptor.uniformBuffer = m_RadiancePropagationUBO;
                                } else {
                                    switch (descriptor.bindingInfo.binding) {
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
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.r).getHandle(), {true, glm::vec4(0.0f)}},    // location = 0 // SH_R
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.g).getHandle(), {true, glm::vec4(0.0f)}},    // location = 1 // SH_G
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.b).getHandle(), {true, glm::vec4(0.0f)}},    // location = 2 // SH_B
                    };

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind the sets and update the data
                    radiancePropagationData.GridSize = grid.size;

                    m_RadiancePropagationUBO->SetData(sizeof(RadianceInjectionUBOData), &radiancePropagationData);

                    // Bind the pipeline
                    m_RPropagationPipeline->Bind(cmdBuffer);

                    // Bind the desc sets
                    RHI::BindDescriptorSets(m_RPropagationPipeline, cmdBuffer, &m_PropagationGPUResources[propagationIdx].PropagationDescriptorSet, 1);

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