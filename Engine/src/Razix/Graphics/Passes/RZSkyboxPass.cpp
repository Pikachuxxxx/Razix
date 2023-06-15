// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSkyboxPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZSkyboxPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            m_CommandBuffers.resize(3);
            for (sz i = 0; i < 3; i++) {
                m_CommandBuffers[i] = RZCommandBuffer::Create();
                m_CommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Skybox Command Buffers"));
            }

            auto skyboxShader           = RZShaderLibrary::Get().getShader("Shader.Builtin.Skybox.rzsf");
            auto proceduralSkyboxShader = RZShaderLibrary::Get().getShader("Shader.Builtin.ProceduralSkybox.rzsf");

            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::FRONT;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = skyboxShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::RZTextureProperties::Format::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            m_Pipeline                          = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Skybox Pipeline"));

            pipelineInfo.shader  = proceduralSkyboxShader;
            m_ProceduralPipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Procedural Skybox Pipeline"));

            m_SkyboxCube = MeshFactory::CreateCube();

            auto& frameDataBlock  = blackboard.get<FrameData>();
            auto& lightProbesData = blackboard.get<GlobalLightProbeData>();
            auto& sceneData       = blackboard.get<SceneData>();
            auto& volumetricData  = blackboard.get<VolumetricCloudsData>();

            framegraph.addCallbackPass(
                "Skybox Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto& data) {
                    builder.setAsStandAlonePass();

                    builder.read(frameDataBlock.frameData);
                    builder.read(lightProbesData.environmentMap);
                    builder.read(lightProbesData.diffuseIrradianceMap);
                    builder.read(lightProbesData.specularPreFilteredMap);
                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);
                    builder.read(volumetricData.noiseTexture);

                    builder.write(sceneData.outputHDR);
                },
                [=](const auto& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_CommandBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];

                    RHI::Begin(cmdBuffer);
                    RAZIX_MARK_BEGIN("Skybox pass", glm::vec4(0.33f, 0.45f, 1.0f, 1.0f));

                    cmdBuffer->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{};
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle(), {false, glm::vec4(0.0f)}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle(), {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();

                        RZDescriptor frame_descriptor{};
                        frame_descriptor.offset              = 0;
                        frame_descriptor.size                = sizeof(GPUFrameData);
                        frame_descriptor.bindingInfo.binding = 0;
                        frame_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        frame_descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        frame_descriptor.uniformBuffer       = frameDataBuffer;

                        m_FrameDataDescriptorSet = RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Set - Skybox"));

                        auto envMap = resources.get<FrameGraph::RZFrameGraphTexture>(lightProbesData.environmentMap).getHandle();

                        RZDescriptor lightProbes_descriptor{};
                        lightProbes_descriptor.bindingInfo.binding = 0;
                        lightProbes_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        lightProbes_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        lightProbes_descriptor.texture             = envMap;

                        m_LightProbesDescriptorSet = RZDescriptorSet::Create({lightProbes_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Env Map - Skybox"));

                        auto noiseTexture = resources.get<FrameGraph::RZFrameGraphTexture>(volumetricData.noiseTexture).getHandle();

                        RZDescriptor volumetric_descriptor{};
                        volumetric_descriptor.bindingInfo.binding = 0;
                        volumetric_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        volumetric_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        volumetric_descriptor.texture             = noiseTexture;

                        m_VolumetricDescriptorSet = RZDescriptorSet::Create({volumetric_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Volumetric"));

                        updatedSets = true;
                    }

                    if (!m_UseProceduralSkybox)
                        m_Pipeline->Bind(cmdBuffer);
                    else
                        m_ProceduralPipeline->Bind(cmdBuffer);

                    m_SkyboxCube->getIndexBuffer()->Bind(cmdBuffer);
                    m_SkyboxCube->getVertexBuffer()->Bind(cmdBuffer);

                    std::vector<RZDescriptorSet*> setsToBindInOrder = {m_FrameDataDescriptorSet};
                    if (!m_UseProceduralSkybox) {
                        setsToBindInOrder.push_back(m_LightProbesDescriptorSet);
                        RHI::BindDescriptorSets(m_Pipeline, cmdBuffer, setsToBindInOrder);
                    } else {
                        // Since no skybox, we update the directional light direction
                        auto lights = scene->GetComponentsOfType<LightComponent>();
                        // We use the first found directional light
                        // TODO: Cache this
                        RZLight dirLight;
                        for (auto& lc: lights) {
                            if (lc.light.getType() == LightType::DIRECTIONAL)
                                dirLight = lc.light;
                            break;
                        }
                        struct PCData
                        {
                            glm::vec3 worldSpaceLightPos;
                        } data;
                        // FIXME: Use direction
                        data.worldSpaceLightPos = dirLight.getPosition();
                        RZPushConstant pc;
                        pc.data        = &data;
                        pc.size        = sizeof(PCData);
                        pc.shaderStage = ShaderStage::PIXEL;

                        RHI::BindPushConstant(m_ProceduralPipeline, cmdBuffer, pc);
                        setsToBindInOrder.push_back(m_VolumetricDescriptorSet);
                        RHI::BindDescriptorSets(m_ProceduralPipeline, cmdBuffer, setsToBindInOrder);
                    }

                    RHI::DrawIndexed(cmdBuffer, m_SkyboxCube->getIndexBuffer()->getCount());

                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();

                    Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());
                    Graphics::RHI::SubmitWork({}, {});
                });
        }

        void RZSkyboxPass::destroy()
        {
            m_Pipeline->Destroy();
            m_ProceduralPipeline->Destroy();
            m_FrameDataDescriptorSet->Destroy();
            m_LightProbesDescriptorSet->Destroy();
            m_VolumetricDescriptorSet->Destroy();
            m_SkyboxCube->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix