// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGBufferPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RZRHI.h"

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

namespace Razix {
    namespace Graphics {

        void RZGBufferPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // First order of business get the shader
            auto shader = RZShaderLibrary::Get().getShader("gbuffer_pass.rzsf");

            for (size_t i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("GBuffer Command Buffers"));
            }

            // Create the descriptor sets to hold the MVP and Material Info

            m_ModelViewProjectionSystemUBO = RZUniformBuffer::Create(sizeof(ModelViewProjectionSystemUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("MVP GBuffer Pass UBO"));

            auto setInfos = shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_VIEW_PROJECTION) {
                            descriptor.uniformBuffer = m_ModelViewProjectionSystemUBO;
                            m_MVPDescriptorSet       = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("MVP GBuffer Pass Set"));
                        }
                    }
                }
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            // Using 32 bit floating point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Graphics::RZTexture::Format::RGBA32F,
                Graphics::RZTexture::Format::RGBA32F,
                Graphics::RZTexture::Format::RGBA32F,
                Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat = Graphics::RZTexture::Format::DEPTH16_UNORM;

            m_Pipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("GBuffer pipeline"));

             blackboard.add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "GBuffer",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, GBufferData& data) {
                    builder.setAsStandAlonePass();

                    data.Normal = builder.create<FrameGraph::RZFrameGraphTexture>("Normal", {FrameGraph::TextureType::Texture_RenderTarget, "Normal", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.Albedo = builder.create<FrameGraph::RZFrameGraphTexture>("Albedo", {FrameGraph::TextureType::Texture_RenderTarget, "Albedo", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.Emissive = builder.create<FrameGraph::RZFrameGraphTexture>("Emissive", {FrameGraph::TextureType::Texture_RenderTarget, "Emissive", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.MetRougAOSpec = builder.create<FrameGraph::RZFrameGraphTexture>("MetRougAOSpec", {FrameGraph::TextureType::Texture_RenderTarget, "MetRougAOSpec", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.Depth = builder.create<FrameGraph::RZFrameGraphTexture>("Depth", {FrameGraph::TextureType::Texture_Depth, "Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH16_UNORM});

                    data.Normal        = builder.write(data.Normal);
                    data.Albedo        = builder.write(data.Albedo);
                    data.Emissive      = builder.write(data.Emissive);
                    data.MetRougAOSpec = builder.write(data.MetRougAOSpec);
                    data.Depth         = builder.write(data.Depth);
                },
                [=](const GBufferData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_CmdBuffers[RZRHI::getSwapchain()->getCurrentImageIndex()];
                    RZRHI::Begin(cmdBuffer);

                    RAZIX_MARK_BEGIN("GBuffer Pass", glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

                    cmdBuffer->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.Normal).getHandle(), {true, glm::vec4(0.0f)}},           // location = 0
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.Albedo).getHandle(), {true, glm::vec4(0.0f)}},           // location = 1
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.Emissive).getHandle(), {true, glm::vec4(0.0f)}},         // location = 2
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.MetRougAOSpec).getHandle(), {true, glm::vec4(0.0f)}},    // location = 3

                    };
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true}};
                    info.extent          = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize          = true;

                    RZRHI::BeginRendering(cmdBuffer, info);

                    m_Pipeline->Bind(cmdBuffer);

                    // Update the ViewProj matrix using the data from the camera
                    m_ModelViewProjSystemUBOData.viewProjection = scene->getSceneCamera().getViewProjection();

                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////
                    auto& group = scene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                        auto& meshes = model.getMeshes();

                        glm::mat4 transform = trans.GetTransform();

                        m_ModelViewProjSystemUBOData.model = transform;
                        m_ModelViewProjectionSystemUBO->SetData(sizeof(ModelViewProjectionSystemUBOData), &m_ModelViewProjSystemUBOData);

                        // Bind IBO and VBO
                        for (auto& mesh: meshes) {
                            mesh->getVertexBuffer()->Bind(cmdBuffer);
                            mesh->getIndexBuffer()->Bind(cmdBuffer);

                            // Combine System Desc sets with material sets and Bind them
                            std::vector<RZDescriptorSet*> SystemMat = {m_MVPDescriptorSet};
                            std::vector<RZDescriptorSet*> MatSets   = mesh->getMaterial()->getDescriptorSets();
                            SystemMat.insert(SystemMat.end(), MatSets.begin(), MatSets.end());
                            Graphics::RZRHI::BindDescriptorSets(m_Pipeline, cmdBuffer, SystemMat);

                            Graphics::RZRHI::DrawIndexed(Graphics::RZRHI::getCurrentCommandBuffer(), mesh->getIndexCount());
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
                        Graphics::RZRHI::BindDescriptorSets(m_Pipeline, cmdBuffer, SystemMat);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                        Graphics::RZRHI::DrawIndexed(Graphics::RZRHI::getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }
                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////

                    RAZIX_MARK_END();
                    RZRHI::EndRendering(cmdBuffer);

                    RZRHI::Submit(cmdBuffer);
                    RZRHI::SubmitWork({}, {});
                });
        }

        void RZGBufferPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix
