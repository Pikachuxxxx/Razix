// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShadowPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
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

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Gfx/Lighting/RZLight.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Gfx {

        void RZShadowPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Load the shader
            auto shader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DepthPreTest);
            auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Pipeline.Shadows";
            pipelineInfo.cullMode            = Gfx::CullMode::Back;
            pipelineInfo.drawType            = Gfx::DrawType::Triangle;
            pipelineInfo.shader              = shader;
            pipelineInfo.depthTestEnabled    = true;
            pipelineInfo.depthWriteEnabled   = true;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthFormat         = {Gfx::TextureFormat::DEPTH32F};
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            framegraph.getBlackboard().add<SimpleShadowPassData>() = framegraph.addCallbackPass<SimpleShadowPassData>(
                "Pass.Builtin.Code.RenderShadows",
                [&](SimpleShadowPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Lighting);

                    RZTextureDesc shadowTextureDesc         = {};
                    shadowTextureDesc.name                  = "ShadowMap";
                    shadowTextureDesc.width                 = kShadowMapSize;
                    shadowTextureDesc.height                = kShadowMapSize;
                    shadowTextureDesc.type                  = TextureType::kDepth;
                    shadowTextureDesc.format                = TextureFormat::DEPTH32F;
                    shadowTextureDesc.initResourceViewHints = ResourceViewHint::kDSV | ResourceViewHint::kSRV;
                    shadowTextureDesc.enableMips            = false;
                    data.shadowMap                          = builder.create<FrameGraph::RZFrameGraphTexture>(shadowTextureDesc.name, CAST_TO_FG_TEX_DESC shadowTextureDesc);

                    data.lightVP = builder.create<FrameGraph::RZFrameGraphBuffer>("LightSpaceMatrix", {"LightSpaceMatrix", sizeof(LightVPUBOData), 0, BufferUsage::PersistentStream});

                    data.shadowMap = builder.write(data.shadowMap);
                    data.lightVP   = builder.write(data.lightVP);
                },
                [=](const SimpleShadowPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Shadows);

                    RAZIX_TIME_STAMP_BEGIN("Shadow Pass");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.RenderShadows", float4(0.65, 0.73, 0.22f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

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

                    float4x4 lightView       = lookAt(dir_light.getPosition(), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
                    float4x4 lightProjection = perspective(60.0f, 1.0f, 0.1f, 100.0f);
                    lightProjection[1][1] *= -1;
                    light_data.lightViewProj = lightProjection * lightView;

                    auto lightVPHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(lightVPHandle)->SetData(sizeof(LightVPUBOData), &light_data);

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        auto descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(data.lightVP)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle();


                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.resolution      = Resolution::kCustom;
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.shadowMap).getHandle(), {true, ClearColorPresets::OpaqueBlack}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    info.layerCount      = 1;
                    RHI::BeginRendering(cmdBuffer, info);

                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Draw calls
                    // Get the meshes from the Scene and render them
                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    // End Rendering
                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZShadowPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Gfx
}    // namespace Razix
