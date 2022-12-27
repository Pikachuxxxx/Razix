// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGIPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Graphics/API/RZRenderContext.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Graphics/Renderers/RZShadowRenderer.h"

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
            auto rsmLightViewProj = RZShadowRenderer::buildCascades(scene->getSceneCamera(), glm::vec3(1.0f), 1, 1.0f, kRSMResolution)[0].viewProjMatrix;

            // RSM pass
            ReflectiveShadowMapData RSM = addRSMPass(framegraph, blackboard, scene, rsmLightViewProj, glm::vec3(1.0f));

            // Add this to the blackboard
            blackboard.add<ReflectiveShadowMapData>(RSM);

            // Radiance Injection Pass
            auto radiance = addRadianceInjectionPass(framegraph, RSM, m_Grid);
        }

        void RZGIPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD;
        }

        ReflectiveShadowMapData RZGIPass::addRSMPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, const glm::mat4& lightViewProj, glm::vec3 lightIntensity)
        {
            auto shader   = RZShaderLibrary::Get().getShader("RSM.rzsf");
            auto setInfos = shader->getSetsCreateInfos();

            m_ModelViewProjectionSystemUBO = RZUniformBuffer::Create(sizeof(ModelViewProjectionSystemUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("MVP GI Pass UBO"));

            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_VIEW_PROJECTION) {
                            descriptor.uniformBuffer = m_ModelViewProjectionSystemUBO;
                            m_MVPDescriptorSet       = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("MVP GI Pass Set"));
                        }
                    }
                }
            }

            // Create the command buffers
            m_RSMCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (uint32_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RSMCmdBuffers[i] = RZCommandBuffer::Create();
                m_RSMCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("RSM pass Command Buffers"));
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            // worldPos, normal, flux, Depth
            pipelineInfo.colorAttachmentFormats = {
                Graphics::RZTexture::Format::RGBA32F,
                Graphics::RZTexture::Format::RGBA32F,
                Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat = Graphics::RZTexture::Format::DEPTH16_UNORM;

            m_RSMPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("RSM pipeline"));

            const auto& data = framegraph.addCallbackPass<ReflectiveShadowMapData>(
                "Reflective Shadow Map",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, ReflectiveShadowMapData& data) {
                    builder.setAsStandAlonePass();

                    // Create the output RTs
                    data.position = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Position", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Position", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.normal = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Normal", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Normal", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.flux = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Flux", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Flux", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Depth", {FrameGraph::TextureType::Texture_Depth, "RSM/Depth", {kRSMResolution, kRSMResolution}, RZTexture::Format::DEPTH16_UNORM});

                    data.position = builder.write(data.position);
                    data.normal   = builder.write(data.normal);
                    data.flux     = builder.write(data.flux);
                    data.depth    = builder.write(data.depth);
                },
                [=](const ReflectiveShadowMapData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_RSMCmdBuffers[RZRenderContext::getSwapchain()->getCurrentImageIndex()];
                    RZRenderContext::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "RSM Pass";
                    } checkpointData;

                    RZRenderContext::SetCmdCheckpoint(cmdBuffer, &checkpointData);

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

                    RZRenderContext::BeginRendering(cmdBuffer, info);

#if 1
                    // Bind the pipeline
                    m_RSMPipeline->Bind(cmdBuffer);

                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////
                    auto& group = scene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                        auto& meshes = model.getMeshes();

                        glm::mat4 transform = trans.GetTransform();

                        m_ModelViewProjSystemUBOData.model          = transform;
                        m_ModelViewProjSystemUBOData.viewProjection = lightViewProj;
                        m_ModelViewProjectionSystemUBO->SetData(sizeof(ModelViewProjectionSystemUBOData), &m_ModelViewProjSystemUBOData);

                        // Bind IBO and VBO
                        for (auto& mesh: meshes) {
                            mesh->getVertexBuffer()->Bind(cmdBuffer);
                            mesh->getIndexBuffer()->Bind(cmdBuffer);

                            // Combine System Desc sets with material sets and Bind them
                            std::vector<RZDescriptorSet*> SystemMat = {m_MVPDescriptorSet};
                            std::vector<RZDescriptorSet*> MatSets   = mesh->getMaterial()->getDescriptorSets();
                            SystemMat.insert(SystemMat.end(), MatSets.begin(), MatSets.end());
                            Graphics::RZRenderContext::BindDescriptorSets(m_RSMPipeline, cmdBuffer, SystemMat);

                            Graphics::RZRenderContext::DrawIndexed(Graphics::RZRenderContext::getCurrentCommandBuffer(), mesh->getIndexCount());
                        }
                    }
                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
                    auto& mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetTransform();

                        m_ModelViewProjSystemUBOData.model = transform;
                        m_ModelViewProjectionSystemUBO->SetData(sizeof(ModelViewProjectionSystemUBOData), &m_ModelViewProjSystemUBOData);

                        // Combine System Desc sets with material sets and Bind them
                        std::vector<RZDescriptorSet*> SystemMat = {m_MVPDescriptorSet};
                        std::vector<RZDescriptorSet*> MatSets   = mrc.Mesh->getMaterial()->getDescriptorSets();
                        SystemMat.insert(SystemMat.end(), MatSets.begin(), MatSets.end());
                        Graphics::RZRenderContext::BindDescriptorSets(m_RSMPipeline, cmdBuffer, SystemMat);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                        Graphics::RZRenderContext::DrawIndexed(Graphics::RZRenderContext::getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }
                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
#endif

                    RAZIX_MARK_END();
                    RZRenderContext::EndRendering(cmdBuffer);

                    RZRenderContext::Submit(cmdBuffer);
                    RZRenderContext::SubmitWork({}, {});
                });

            return data;
        }

        LightPropagationVolumesData RZGIPass::addRadianceInjectionPass(FrameGraph::RZFrameGraph& framegraph, const ReflectiveShadowMapData& RSM, const Maths::RZGrid& grid)
        {
            // Load the shader properly
            auto shader = RZShaderLibrary::Get().getShader("lpv_radiance_injection.rzsf");

            // Get the setinfo ==> allocate UBOs and Textures and bind them to the descriptor sets

            m_RadianceInjectionUBO = RZUniformBuffer::Create(sizeof(RadianceInjectionUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("RI UBO"));

            // Create command buffers
            m_RadianceInjectionCmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (uint32_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_RadianceInjectionCmdBuffers[i] = RZCommandBuffer::Create();
                m_RadianceInjectionCmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Radiance Injection Command Buffers"));
            }

            // create pipeline
            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::POINT;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            // Depth, worldPos, normal, flux
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTexture::Format::RGBA32F, Graphics::RZTexture::Format::RGBA32F, Graphics::RZTexture::Format::RGBA32F};

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
                    data.r = builder.create<FrameGraph::RZFrameGraphTexture>("SH/R", {FrameGraph::TextureType::Texture_2D, "SH/R", {grid.size.x, grid.size.y}, RZTexture::Format::RGBA32F, grid.size.z});
                    data.g = builder.create<FrameGraph::RZFrameGraphTexture>("SH/G", {FrameGraph::TextureType::Texture_2D, "SH/G", {grid.size.x, grid.size.y}, RZTexture::Format::RGBA32F, grid.size.z});
                    data.b = builder.create<FrameGraph::RZFrameGraphTexture>("SH/B", {FrameGraph::TextureType::Texture_2D, "SH/B", {grid.size.x, grid.size.y}, RZTexture::Format::RGBA32F, grid.size.z});

                    data.r = builder.write(data.r);
                    data.g = builder.write(data.g);
                    data.b = builder.write(data.b);
                },
                [=](const LightPropagationVolumesData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

#if 1
                    auto cmdBuffer = m_RadianceInjectionCmdBuffers[RZRenderContext::getSwapchain()->getCurrentImageIndex()];
                    RZRenderContext::Begin(cmdBuffer);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "Radiance Injection Pass";
                    } checkpointData;

                    RZRenderContext::SetCmdCheckpoint(cmdBuffer, &checkpointData);

                    RAZIX_MARK_BEGIN("Radiance Injection", glm::vec4(.53f, .45f, .76f, 1.0f))

                    cmdBuffer->UpdateViewport(kRSMResolution, kRSMResolution);

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
                    info.layerCount       = grid.size.z;
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.r).getHandle(), {true, glm::vec4(0.0f)}},    // location = 0 // SH_R
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.g).getHandle(), {true, glm::vec4(0.0f)}},    // location = 1 // SH_G
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.b).getHandle(), {true, glm::vec4(0.0f)}},    // location = 2 // SH_B
                    };

                    RZRenderContext::BeginRendering(cmdBuffer, info);

                    // Bind the sets and update the data
                    radianceInjectionData.RSMResolution = kRSMResolution;
                    radianceInjectionData.MinCorner     = grid.aabb.min;
                    radianceInjectionData.GridSize      = grid.size;
                    radianceInjectionData.CellSize      = grid.cellSize;

                    m_RadianceInjectionUBO->SetData(sizeof(RadianceInjectionUBOData), &radianceInjectionData);

                    // Bind the pipeline
                    m_RIPipeline->Bind(cmdBuffer);

                    // Bind the desc sets
                    RZRenderContext::BindDescriptorSets(m_RIPipeline, cmdBuffer, &m_RIDescriptorSet, 1);

                    RZRenderContext::Draw(cmdBuffer, kNumVPL);

                    RAZIX_MARK_END();
                    RZRenderContext::EndRendering(cmdBuffer);

                    RZRenderContext::Submit(cmdBuffer);
                    RZRenderContext::SubmitWork({}, {});
#endif
                });
            return data;
        }
    }    // namespace Graphics
}    // namespace Razix