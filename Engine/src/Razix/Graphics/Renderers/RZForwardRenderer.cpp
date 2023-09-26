// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZForwardRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZFramebuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZRenderPass.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZMesh.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZForwardRenderer::Init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererName = "Forward Renderer";

            // Init the width and height of RT
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // This is a override shader that won't be used
            // Giving the shader to the renderer is not something I can think will be useful I think material will decide that
            // So what does the renderer do then? it's job is to enforce some rules on the shader (on the UBO data and samplers) and it handles how it updates the data in a way that is appropriate for that renderer to operate
            m_OverrideGlobalRHIShader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);

            // FrameBlock Descriptor Set

            // 2. Lighting data
            m_ForwardLightsUBO = RZResourceManager::Get().createUniformBuffer({"Forward Renderer Light Data", sizeof(GPULightsData), nullptr});

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = RZResourceManager::Get().getShaderResource(m_OverrideGlobalRHIShader)->getDescriptorsPerHeapMap();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                        if (setInfo.first == BindingTable_System::SET_IDX_SYSTEM_START) {
                            //descriptor.uniformBuffer = m_SystemMVPUBO;
                            //m_MVPDescriptorSet       = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_VIEW_PROJECTION"));
                        } else if (setInfo.first == BindingTable_System::SET_IDX_LIGHTING_DATA) {
                            descriptor.uniformBuffer = m_ForwardLightsUBO;
                            m_GPULightsDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_LIGHTING_DATA"));
                        }
                    }
                }
            }

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Forward Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA8};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);
        }

        void RZForwardRenderer::Begin(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            RAZIX_MARK_BEGIN("Forward Lighting Pass", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            auto& registry = scene->getRegistry();

            // Upload the lights data
            GPULightsData gpuLightsData;
            auto          group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
            for (auto entity: group) {
                const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                // Set the Position of the light using this transform component
                lightComponent.light.getLightData().position = transformComponent.Translation;
                lightComponent.light.setDirection(glm::vec3(glm::degrees(transformComponent.Rotation.x), glm::degrees(transformComponent.Rotation.y), glm::degrees(transformComponent.Rotation.z)));
                gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                gpuLightsData.numLights++;
            }
            RZResourceManager::Get().getUniformBufferResource(m_ForwardLightsUBO)->SetData(sizeof(GPULightsData), &gpuLightsData);

            gpuLightsData = {};

            m_CurrentScene = scene;
        }

        void RZForwardRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind the pipeline
            RHI::BindPipeline(m_Pipeline, cmdBuffer);

            auto mesh_group = m_CurrentScene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
            for (auto entity: mesh_group) {
                // Draw the mesh renderer components
                const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = mesh_trans.GetGlobalTransform();

                //-----------------------------
                // Get the shader from the Mesh Material later
                // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                auto& modelMatrix = RZResourceManager::Get().getShaderResource(m_OverrideGlobalRHIShader)->getPushConstants()[0];

                struct PCD
                {
                    glm::mat4 mat;
                } pcData{};
                pcData.mat       = transform;
                modelMatrix.data = &pcData;
                modelMatrix.size = sizeof(PCD);

                // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                Graphics::RHI::BindPushConstant(m_Pipeline, cmdBuffer, modelMatrix);
                //-----------------------------

                mrc.Mesh->getMaterial()->Bind();

                // Combine System Desc sets with material sets and Bind them
                std::vector<RZDescriptorSet*> setsToBindInOrder = {m_FrameDataSet, mrc.Mesh->getMaterial()->getDescriptorSet(), m_GPULightsDescriptorSet /*, m_CSMSet*/};
                Graphics::RHI::BindUserDescriptorSets(m_Pipeline, cmdBuffer, setsToBindInOrder);

                mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
            }
        }

        void RZForwardRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RHI::EndRendering(Graphics::RHI::GetCurrentCommandBuffer());

            RAZIX_MARK_END();
        }

        void RZForwardRenderer::Resize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;
        }

        void RZForwardRenderer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the resources first
            RZResourceManager::Get().destroyUniformBuffer(m_ForwardLightsUBO);
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            m_GPULightsDescriptorSet->Destroy();
            m_CSMSet->Destroy();
            m_FrameDataSet->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix
