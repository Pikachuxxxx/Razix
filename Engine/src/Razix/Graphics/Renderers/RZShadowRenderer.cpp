// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShadowRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        //-------------------------------------------------------------
        // IRZRenderer

        void RZShadowRenderer::Init()
        {
            m_LightViewProjUBO = RZUniformBuffer::Create(sizeof(LightVPUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("LightViewProj"));

            // Build the command buffers
            for (sz i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Shadpw mapping Command Buffers"));
            }
        }

        void RZShadowRenderer::Begin(RZScene* scene)
        {
        }

        void RZShadowRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
        }

        void RZShadowRenderer::End()
        {
        }

        void RZShadowRenderer::Resize(u32 width, u32 height)
        {
        }

        void RZShadowRenderer::Destroy()
        {
            m_LightViewProjUBO->Destroy();
            m_Pipeline->Destroy();
            if (m_FrameDataSet)
                m_FrameDataSet->Destroy();
            m_LVPSet->Destroy();
        }

        void RZShadowRenderer::SetFrameDataHeap(RZDescriptorSet* frameDataSet)
        {
            m_FrameDataSet = frameDataSet;
        }

        //-------------------------------------------------------------
        // IRZPass

        void RZShadowRenderer::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Load the shader
            auto  shader   = RZShaderLibrary::Get().getShader("shadow_mapping.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = m_LightViewProjUBO;
                }
                m_LVPSet = (Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow Depth pass set")));
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::BACK;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthFormat         = {Graphics::RZTexture::Format::DEPTH32F};
            m_Pipeline                       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow Pass Pipeline"));

            blackboard.add<SimpleShadowPassData>() = framegraph.addCallbackPass<SimpleShadowPassData>(
                "Simple Shadow map pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SimpleShadowPassData& data) {
                    builder.setAsStandAlonePass();

                    data.shadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("Shadow map", {FrameGraph::TextureType::Texture_Depth, "Shadow map", {kShadowMapSize, kShadowMapSize}, RZTexture::Format::DEPTH32F});

                    data.lightVP = builder.create<FrameGraph::RZFrameGraphBuffer>("LightSpaceMatrix", {"LightSpaceMatrix", sizeof(LightVPUBOData)});

                    data.shadowMap = builder.write(data.shadowMap);
                    data.lightVP   = builder.write(data.lightVP);
                },
                [=](const SimpleShadowPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuf = m_MainCommandBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];

                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Shadow Pass", glm::vec4(0.65, 0.73, 0.22f, 1.0f));

                    // Update Viewport and Scissor Rect
                    cmdBuf->UpdateViewport(kShadowMapSize, kShadowMapSize);

                    LightVPUBOData light_data{};
                    // Get the Light direction
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
                    RZLight dir_light;
                    for (auto& light: lights) {
                        if (light.light.getType() == LightType::DIRECTIONAL) {
                            dir_light = light.light;
                            break;
                        }
                    }

                    glm::mat4 lightView  = glm::lookAt(dir_light.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    float     near_plane = -100.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    light_data.lightViewProj = lightProjection * lightView;

                    resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle()->SetData(sizeof(LightVPUBOData), &light_data);

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.shadowMap).getHandle(), {true, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    info.layerCount      = 1;
                    info.resize          = false;
                    RHI::BeginRendering(cmdBuf, info);

                    m_Pipeline->Bind(cmdBuf);

                    RHI::BindDescriptorSets(m_Pipeline, cmdBuf, &m_LVPSet, 1);

                    // Draw calls
                    // Get the meshes and the models from the Scene and render them

                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////
                    auto group = scene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                        auto& meshes = model.getMeshes();

                        glm::mat4 transform = trans.GetTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = shader->getPushConstants()[0];

                        struct PCD
                        {
                            glm::mat4 mat;
                        } pcData;
                        pcData.mat       = transform;
                        modelMatrix.data = &pcData;
                        modelMatrix.size = sizeof(PCD);

                        // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                        Graphics::RHI::BindPushConstant(m_Pipeline, cmdBuf, modelMatrix);
                        //-----------------------------

                        m_LightViewProjUBO->SetData(sizeof(LightVPUBOData), &light_data);

                        // Bind IBO and VBO
                        for (auto& mesh: meshes) {
                            mesh->getVertexBuffer()->Bind(cmdBuf);
                            mesh->getIndexBuffer()->Bind(cmdBuf);

                            Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mesh->getIndexCount());
                        }
                    }
                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        if (mrc.Mesh == nullptr)
                            continue;

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = shader->getPushConstants()[0];

                        struct PCD
                        {
                            glm::mat4 mat;
                        } pcData;
                        pcData.mat       = transform;
                        modelMatrix.data = &pcData;
                        modelMatrix.size = sizeof(PCD);

                        // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                        Graphics::RHI::BindPushConstant(m_Pipeline, cmdBuf, modelMatrix);
                        //-----------------------------

                        m_LightViewProjUBO->SetData(sizeof(LightVPUBOData), &light_data);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuf);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuf);

                        Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }
                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////

                    // End Rendering
                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();

                    // End Command Buffer Recording
                    RHI::Submit(cmdBuf);

                    // Submit the work for execution + synchronization
                    // Signal a passDoneSemaphore only on the last cascade pass
                    RHI::SubmitWork({}, {/*PassDoneSemaphore*/});
                });
        }

        void RZShadowRenderer::destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix