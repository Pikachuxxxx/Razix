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
            auto RSM = addRSMPass(framegraph, blackboard, scene, rsmLightViewProj, glm::vec3(1.0f));

            // Add this to the blackboard
            blackboard.add<ReflectiveShadowMapData>(RSM);
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
            m_CmdBuffers.resize(RAZIX_MAX_SWAP_IMAGES_COUNT);
            for (uint32_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("GI Pass Command Buffers"));
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            // Depth, worldPos, normal, flux
            pipelineInfo.attachmentFormats = {Graphics::RZTexture::Format::RGBA32F, Graphics::RZTexture::Format::RGBA32F, Graphics::RZTexture::Format::RGBA32F, Graphics::RZTexture::Format::DEPTH16_UNORM};

            m_RSMPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("RSM pipeline"));

            const auto data = framegraph.addCallbackPass<ReflectiveShadowMapData>(
                "Reflective Shadow Map",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, ReflectiveShadowMapData& data) {
                    builder.setAsStandAlonePass();

                    // Create the output RTs
                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Depth", {FrameGraph::TextureType::Texture_Depth, "RSM/Depth", {kRSMResolution, kRSMResolution}, RZTexture::Format::DEPTH16_UNORM});

                    data.position = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Position", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Position", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.normal = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Normal", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Normal", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.flux = builder.create<FrameGraph::RZFrameGraphTexture>("RSM/Flux", {FrameGraph::TextureType::Texture_RenderTarget, "RSM/Flux", {kRSMResolution, kRSMResolution}, RZTexture::Format::RGBA32F});

                    data.depth    = builder.write(data.depth);
                    data.position = builder.write(data.position);
                    data.normal   = builder.write(data.normal);
                    data.flux     = builder.write(data.flux);
                },
                [=](const ReflectiveShadowMapData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_CmdBuffers[RZRenderContext::getSwapchain()->getCurrentImageIndex()];
                    RZRenderContext::Begin(cmdBuffer);

                    RAZIX_MARK_BEGIN("ReflectiveShadowMap", glm::vec4(.23f, .45f, .76f, 1.0f))

                    cmdBuffer->UpdateViewport(kRSMResolution, kRSMResolution);

                    RenderingInfo info{};
                    info.attachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.position).getHandle(), {true, glm::vec4(0.0f)}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.flux).getHandle(), {true, glm::vec4(0.0f)}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.normal).getHandle(), {true, glm::vec4(0.0f)}},
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true}},
                    };
                    info.extent = {kRSMResolution, kRSMResolution};
                    info.resize = false;

                    RZRenderContext::BeginRendering(cmdBuffer, info);

                    // Bind the pipeline
                    m_RSMPipeline->Bind(cmdBuffer);

                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////
                    auto group = scene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                        auto& meshes = model.getMeshes();

                        glm::mat4 transform = trans.GetTransform();

                        m_ModelViewProjSystemUBOData.model = transform;
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
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
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

                    RAZIX_MARK_END();
                    RZRenderContext::EndRendering(cmdBuffer);

                    RZRenderContext::Submit(cmdBuffer);
                    RZRenderContext::SubmitWork({}, {});
                });

            return data;
        }
    }    // namespace Graphics
}    // namespace Razix