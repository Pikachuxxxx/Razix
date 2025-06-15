// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGaussianBlurPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Gfx {

        struct GaussianBlurOutput
        {
            RZFrameGraphResource blur;
        };

        struct GaussianBlurPCData
        {
            u32       tapFilter;
            f32       blurRadius;
            float2 direction;
        };

        void RZGaussianBlurPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto gaussiabBlurShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GaussianBlur);

            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Gaussian Blur FX Pipeline";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = gaussiabBlurShader;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            if (m_IsTwoPass) {
                /**
                 * In a two pass Gaussian filter we do a Horizontal and then a Vertical blur on the input texture
                 */
                auto blurH      = addBlurPass(framegraph, scene, settings, m_InputTexture, m_BlurRadius, m_FilterTap, GaussianDirection::Horizontal);
                m_OutputTexture = addBlurPass(framegraph, scene, settings, blurH, m_BlurRadius, m_FilterTap, GaussianDirection::Vertical);
            } else
                m_OutputTexture = addBlurPass(framegraph, scene, settings, m_InputTexture, m_BlurRadius, m_FilterTap, m_Direction);
        }

        void RZGaussianBlurPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        RZFrameGraphResource RZGaussianBlurPass::addBlurPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings, RZFrameGraphResource inputTexture, f32 blurRadius, GaussianTap filterTap, GaussianDirection direction)
        {
            auto gaussiabBlurShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GaussianBlur);

            auto& pass = framegraph.addCallbackPass<GaussianBlurOutput>(
                "Pass.Builtin.Code.FX.GaussianBlur",
                [&](GaussianBlurOutput& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{};
                    textureDesc.name       = "GaussianBlurredTex";
                    textureDesc.width      = g_ResolutionToExtentsMap[Resolution::k1440p].x;
                    textureDesc.height     = g_ResolutionToExtentsMap[Resolution::k1440p].y;
                    textureDesc.type       = TextureType::k2D;
                    textureDesc.format     = TextureFormat::RGBA16F;
                    textureDesc.enableMips = false;

                    data.blur = builder.create<RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    builder.read(inputTexture);
                    data.blur = builder.write(data.blur);
                },
                [=](const GaussianBlurOutput& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("GaussianBlur");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.FX.GaussianBlur", float4(178.0f, 190.0f, 181.0f, 255.0f) / 255.0f);

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<RZFrameGraphTexture>(data.blur).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    if (RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(gaussiabBlurShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["inputTexture"];
                        if (descriptor)
                            descriptor->texture = resources.get<RZFrameGraphTexture>(inputTexture).getHandle();

                        RZResourceManager::Get().getShaderResource(gaussiabBlurShader)->updateBindVarsHeaps();
                    }

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    GaussianBlurPCData pcData{};
                    pcData.blurRadius = blurRadius;
                    pcData.direction  = direction == GaussianDirection::Horizontal ? float2(1, 0) : float2(0, 1);
                    pcData.tapFilter  = (u32) filterTap;

                    RZPushConstant pc;
                    pc.size        = sizeof(GaussianBlurPCData);
                    pc.data        = &pcData;
                    pc.shaderStage = ShaderStage::kPixel;
                    RHI::BindPushConstant(m_Pipeline, RHI::GetCurrentCommandBuffer(), pc);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });

            return pass.blur;
        }
    }    // namespace Gfx
}    // namespace Razix