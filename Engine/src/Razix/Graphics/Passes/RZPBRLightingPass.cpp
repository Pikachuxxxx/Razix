// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRLightingPass.h"

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
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZPBRLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            m_CommandBuffers.resize(3);
            for (sz i = 0; i < 3; i++) {
                m_CommandBuffers[i] = RZCommandBuffer::Create();
                m_CommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("PBR Command Buffers"));
            }

            auto pbrShader = RZShaderLibrary::Get().getShader("pbr_lighting.rzsf");

            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::FRONT;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = pbrShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::RZTexture::Format::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("PBR Pipeline"));

            auto& frameDataBlock       = blackboard.get<FrameData>();
            auto& sceneLightsDataBlock = blackboard.get<SceneLightsData>();

            blackboard.add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "PBR Lighting Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneData& data) {
                    builder.setAsStandAlonePass();

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR RT", {FrameGraph::TextureType::Texture_RenderTarget, "Scene HDR RT", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32F});

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("Scene Depth", {FrameGraph::TextureType::Texture_Depth, "Scene Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH16_UNORM});

                    data.outputHDR = builder.write(data.outputHDR);
                    data.depth     = builder.write(data.depth);

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneLightsDataBlock.lightsDataBuffer);
                },
                [=](const SceneData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_CommandBuffers[RHI::getSwapchain()->getCurrentImageIndex()];

                    RHI::Begin(cmdBuffer);
                    RAZIX_MARK_BEGIN("PBR pass", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    cmdBuffer->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{};
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {true, scene->getSceneCamera().getBgColor()}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::getCurrentCommandBuffer(), info);

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

                        m_FrameDataDescriptorSet = RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Set - PBR"));

                        auto lightsDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(sceneLightsDataBlock.lightsDataBuffer).getHandle();

                        RZDescriptor lightsData_descriptor{};
                        lightsData_descriptor.offset              = 0;
                        lightsData_descriptor.size                = sizeof(GPULightsData);
                        lightsData_descriptor.bindingInfo.binding = 0;
                        lightsData_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        lightsData_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        lightsData_descriptor.uniformBuffer       = lightsDataBuffer;

                        m_SceneLightsDataDescriptorSet = RZDescriptorSet::Create({lightsData_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Scene Lights Set - PBR"));

                        updatedSets = true;
                    }

                    m_Pipeline->Bind(cmdBuffer);

                    scene->drawScene(m_Pipeline, m_FrameDataDescriptorSet, m_SceneLightsDataDescriptorSet);

                    RHI::EndRendering(cmdBuffer);

                    Graphics::RHI::Submit(Graphics::RHI::getCurrentCommandBuffer());
                    Graphics::RHI::SubmitWork({}, {});
                });
        }

        void RZPBRLightingPass::destroy()
        {
            m_Pipeline->Destroy();
            m_FrameDataDescriptorSet->Destroy();
            m_SceneLightsDataDescriptorSet->Destroy();
        }

    }    // namespace Graphics
}    // namespace Razix