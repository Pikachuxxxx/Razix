// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZHelloTextureTestPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

        void RZHelloTextureTestPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto shader = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::HelloTextureTest);

            RZPipelineDesc pipelineInfo{};
            // Build the pipeline here for this pass
            pipelineInfo.name                   = "[Test] Pipeline.HelloTexture";
            pipelineInfo.shader                 = shader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::SCREEN};
#ifdef __APPLE__    // Metal cannot draw without a depth attachment
            pipelineInfo.depthFormat = TextureFormat::DEPTH16_UNORM;
#endif
            pipelineInfo.cullMode            = Gfx::CullMode::None;
            pipelineInfo.drawType            = Gfx::DrawType::Triangle;
            pipelineInfo.depthTestEnabled    = false;
            pipelineInfo.depthWriteEnabled   = false;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Import a test texture
            RZTextureDesc testTexDesc{};
            testTexDesc.name       = "TestCheckerTexture";
            testTexDesc.enableMips = false;
            testTexDesc.filePath   = "//RazixContent/Textures/TestCheckerMap.png";
            testTexDesc.filtering  = {Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear};
            testTexDesc.wrapping   = Wrapping::kRepeat;
            m_TestTextureHandle    = RZResourceManager::Get().createTexture(testTexDesc);

            struct HelloTexturePassData
            {
                FrameGraph::RZFrameGraphResource Depth;
            };

            framegraph.addCallbackPass<HelloTexturePassData>(
                "[Test] Pass.Builtin.Code.HelloTexture",
                [&](HelloTexturePassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

#ifdef __APPLE__    // Metal cannot draw without a depth attachment
                    RZTextureDesc depthTextureDesc;
                    depthTextureDesc.name      = "SceneDepth";
                    depthTextureDesc.width     = RZApplication::Get().getWindow()->getWidth();
                    depthTextureDesc.height    = RZApplication::Get().getWindow()->getHeight();
                    depthTextureDesc.format    = TextureFormat::DEPTH16_UNORM;
                    depthTextureDesc.filtering = {Filtering::Mode::kFilterModeNearest, Filtering::Mode::kFilterModeNearest},
                    depthTextureDesc.type      = TextureType::kDepth;
                    data.Depth                 = builder.create<FrameGraph::RZFrameGraphTexture>(depthTextureDesc.name, CAST_TO_FG_TEX_DESC depthTextureDesc);

                    data.Depth = builder.write(data.Depth);
#endif
                },
                [=](const HelloTexturePassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("[Test] Hello Texture Pass");
                    RAZIX_MARK_BEGIN("[Test] Pass.Builtin.Code.HelloTexture", Utilities::GenerateHashedColor4(22u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{Gfx::RHI::GetSwapchain()->GetCurrentImage(), {true, ClearColorPresets::OpaqueBlack}}};
#ifdef __APPLE__    // Metal cannot draw without a depth attachment
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
#endif
                    info.resize = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind descriptor sets and resources
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["g_TestTexture"];    // Must match the name in shader
                        // Sampler is static and set on the RootSignature Directly
                        if (descriptor)
                            descriptor->texture = m_TestTextureHandle;

                        // Vulkan will create a default sampler if not found

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    auto& sceneDrawParams = Gfx::RZResourceManager::Get().getShaderResource(shader)->getSceneDrawParams();
                    Gfx::RHI::BindUserDescriptorSets(m_Pipeline, cmdBuffer, sceneDrawParams.userSets, 0);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Draw 3 vertices
                    Gfx::RHI::Draw(cmdBuffer, 3);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZHelloTextureTestPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyTexture(m_TestTextureHandle);
        }
    }    // namespace Gfx
}    // namespace Razix
