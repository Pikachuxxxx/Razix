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

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

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

            for (sz i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("GBuffer Command Buffers"));
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = true;
            // Using 32 bit f32ing point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F};
            pipelineInfo.depthFormat = Graphics::RZTextureProperties::Format::DEPTH16_UNORM;

            m_Pipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("GBuffer pipeline"));

            auto& frameDataBlock = blackboard.get<FrameData>();

            blackboard.add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "GBuffer",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, GBufferData& data) {
                    builder.setAsStandAlonePass();

                    data.Normal = builder.create<FrameGraph::RZFrameGraphTexture>("Normal", {FrameGraph::TextureType::Texture_RenderTarget, "Normal", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::RGBA32F});

                    data.Albedo = builder.create<FrameGraph::RZFrameGraphTexture>("Albedo", {FrameGraph::TextureType::Texture_RenderTarget, "Albedo", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::RGBA32F});

                    data.Emissive = builder.create<FrameGraph::RZFrameGraphTexture>("Emissive", {FrameGraph::TextureType::Texture_RenderTarget, "Emissive", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::RGBA32F});

                    data.MetRougAOSpec = builder.create<FrameGraph::RZFrameGraphTexture>("MetRougAOSpec", {FrameGraph::TextureType::Texture_RenderTarget, "MetRougAOSpec", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::RGBA32F});

                    data.Depth = builder.create<FrameGraph::RZFrameGraphTexture>("Depth", {FrameGraph::TextureType::Texture_Depth, "Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::DEPTH16_UNORM});

                    data.Normal        = builder.write(data.Normal);
                    data.Albedo        = builder.write(data.Albedo);
                    data.Emissive      = builder.write(data.Emissive);
                    data.MetRougAOSpec = builder.write(data.MetRougAOSpec);
                    data.Depth         = builder.write(data.Depth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const GBufferData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuffer = m_CmdBuffers[RHI::GetSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuffer);

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
                    info.resize          = false;

                    RHI::BeginRendering(cmdBuffer, info);

                    m_Pipeline->Bind(cmdBuffer);

                    // Set the Descriptor Set once rendering starts
                    auto        frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();
                    static bool updatedSets     = false;
                    if (!updatedSets) {
                        RZDescriptor descriptor{};
                        descriptor.offset              = 0;
                        descriptor.size                = sizeof(GPUFrameData);
                        descriptor.bindingInfo.binding = 0;
                        descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        descriptor.uniformBuffer       = frameDataBuffer;
                        m_FrameDataSet                 = RZDescriptorSet::Create({descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("FrameDataSet GBuffer"));

                        updatedSets = true;
                    }

                    // TODO: Use scene to draw geometry

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
                        } pcData;
                        pcData.mat       = transform;
                        modelMatrix.data = &pcData;
                        modelMatrix.size = sizeof(PCD);

                        // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                        Graphics::RHI::BindPushConstant(m_Pipeline, cmdBuffer, modelMatrix);
                        //-----------------------------

                        // Combine System Desc sets with material sets and Bind them
                        std::vector<RZDescriptorSet*> setsToBindInOrder = {m_FrameDataSet, mrc.Mesh->getMaterial()->getDescriptorSet()};
                        Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuffer, setsToBindInOrder);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                        Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////

                    RAZIX_MARK_END();
                    RHI::EndRendering(cmdBuffer);

                    RHI::Submit(cmdBuffer);
                    RHI::SubmitWork({}, {});
                });
        }

        void RZGBufferPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix
