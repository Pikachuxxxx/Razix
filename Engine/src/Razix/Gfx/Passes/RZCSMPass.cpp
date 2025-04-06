// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCSMPass.h"

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

#include "Razix/Gfx/Passes/Data/FrameData.h"
#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {
    namespace Gfx {

        void RZCSMPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            /**
             * In CSM we render to a TextureArray Depth map, we select the render layer using gl_Layer or API (setCurrentArrayLayer), but the shaders remain the same as normal shadow map generation
             */
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::CSM);

            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Pipeline.CSM";
            pipelineInfo.cullMode            = Gfx::CullMode::Back;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.drawType            = Gfx::DrawType::Triangle;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthFormat         = {Gfx::TextureFormat::DEPTH32F};
            pipelineInfo.depthTestEnabled    = true;
            pipelineInfo.depthWriteEnabled   = true;
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& csmData = framegraph.getBlackboard().add<CSMData>();

            CascadeSubPassData cascadeSubpassData{-1};
            for (u32 i = 0; i < kNumCascades; i++) {
                cascadeSubpassData = addCascadePass(framegraph, scene, settings, cascadeSubpassData, i);
            }
            csmData.cascadedShadowMaps = cascadeSubpassData.cascadeOutput;

            // Since the above texture passes are cascaded we do an extra pass to constantly update the data into a buffer after all the cascade calculations are done whilst filling the TextureArray2D
            auto& pass = framegraph.addCallbackPass<CSMData>(
                "Upload Cascade Matrices (post CSM calculation)",
                [&](CSMData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();
                    // Create the final cascaded VP data here after rendering the depth texture array, in fact this pass can be parallelized before the we render the depth maps
                    RZBufferDesc bufferDesc = {};
                    bufferDesc.name         = "CB_CascadesMatrixData";
                    bufferDesc.size         = sizeof(CascadesMatrixData);
                    bufferDesc.data         = nullptr;
                    bufferDesc.usage        = BufferUsage::PersistentStream;

                    data.viewProjMatrices = builder.create<FrameGraph::RZFrameGraphBuffer>("CB_CascadesMatrixData", CAST_TO_FG_BUF_DESC bufferDesc);

                    data.viewProjMatrices = builder.write(data.viewProjMatrices);
                },
                [=](const CSMData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    CascadesMatrixData uboData{};
                    for (u32 i = 0; i < kNumCascades; ++i) {
                        uboData.splitDepth[i]       = m_Cascades[i].splitDepth;
                        uboData.viewProjMatrices[i] = m_Cascades[i].viewProjMatrix;
                    }
                    auto vpbufferHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.viewProjMatrices).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(vpbufferHandle)->SetData(sizeof(CascadesMatrixData), &uboData);
                });
            csmData.viewProjMatrices = pass.viewProjMatrices;
        }

        void RZCSMPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        void RZCSMPass::updateCascades(Razix::RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
            auto dirLight = scene->GetComponentsOfType<LightComponent>();

            //auto&         registry   = scene->getRegistry();
            //auto          cameraView = registry.view<CameraComponent>();
            //RZSceneCamera cam;
            //if (!cameraView.empty()) {
            //    // By using front we get the one and only or the first one in the list of camera entities
            //    cam = cameraView.get<CameraComponent>(cameraView.front()).Camera;
            //}
            //m_Cascades = buildCascades(cam, -dirLight[0].light.getPosition(), kNumCascades, 0.75f, kShadowMapSize);

            m_Cascades = buildCascades(scene->getSceneCamera(), -dirLight[0].light.getPosition(), kNumCascades, kSplitLambda, kShadowMapSize);
        }

        std::vector<Cascade> RZCSMPass::buildCascades(RZSceneCamera camera, float3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // [Reference] https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/

            const f32 nearClip  = camera.getPerspectiveNearClip();
            const f32 farClip   = camera.getPerspectiveFarClip();
            const f32 clipRange = farClip - nearClip;

            // Get the cascade splits
            const auto cascadeSplits = buildCascadeSplits(numCascades, lambda, camera.getPerspectiveNearClip(), clipRange);
            const auto invViewProj   = inverse((camera.getProjection() * camera.getViewMatrix()));    // This is causing a flip

            auto lastSplitDist = 0.0f;

            std::vector<Cascade> cascades(numCascades);
            for (u32 i{0}; i < cascades.size(); ++i) {
                const auto splitDist = cascadeSplits[i];

                cascades[i] = {
                    (camera.getPerspectiveNearClip() + splitDist * clipRange),    // -1.0f is causing a flip
                    buildDirLightMatrix(invViewProj, dirLightDirection, shadowMapSize, splitDist, lastSplitDist),
                };
                lastSplitDist = splitDist;
            }
            return cascades;
        }

        std::vector<f32> RZCSMPass::buildCascadeSplits(u32 numCascades, f32 lambda, f32 nearPlane, f32 clipRange)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            constexpr auto kMinDistance = 0.0f, kMaxDistance = 1.0f;

            const auto minZ = nearPlane + kMinDistance * clipRange;
            const auto maxZ = nearPlane + kMaxDistance * clipRange;

            const auto range = maxZ - minZ;
            const auto ratio = maxZ / minZ;

            std::vector<f32> cascadeSplits(numCascades);
            // Calculate split depths based on view camera frustum
            // Based on method presented in:
            // https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
            for (u32 i{0}; i < cascadeSplits.size(); ++i) {
                const auto p       = static_cast<f32>(i + 1) / cascadeSplits.size();
                const auto log     = minZ * std::pow(ratio, p);
                const auto uniform = minZ + range * p;
                const auto d       = lambda * (log - uniform) + uniform;
                cascadeSplits[i]   = (d - nearPlane) / clipRange;
            }
            return cascadeSplits;
        }

        Razix::Gfx::FrustumCorners RZCSMPass::buildFrustumCorners(const float4x4& inversedViewProj, f32 splitDist, f32 lastSplitDist)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // clang-format off
            FrustumCorners frustumCorners{
                // Near
                float3{-1.0f,  1.0f, -1.0f}, // TL
                float3{ 1.0f,  1.0f, -1.0f}, // TR
                float3{ 1.0f, -1.0f, -1.0f}, // BR
                float3{-1.0f, -1.0f, -1.0f}, // BL
                // Far
                float3{-1.0f,  1.0f,  1.0f}, // TL
                float3{ 1.0f,  1.0f,  1.0f}, // TR
                float3{ 1.0f, -1.0f,  1.0f}, // BR
                float3{-1.0f, -1.0f,  1.0f}  // BL
            };
            // clang-format on

            // Project frustum corners into world space
            for (auto& p: frustumCorners) {
                const auto temp = inversedViewProj * float4{p, 1.0f};
                p               = float3{temp} / temp.w;
            }
            for (u32 i{0}; i < 4; ++i) {
                const auto cornerRay     = frustumCorners[i + 4] - frustumCorners[i];
                const auto farCornerRay  = cornerRay * splitDist;
                frustumCorners[i + 4]    = frustumCorners[i] + farCornerRay;
                const auto nearCornerRay = cornerRay * lastSplitDist;
                frustumCorners[i]        = frustumCorners[i] + nearCornerRay;
            }
            return frustumCorners;
        }

        auto RZCSMPass::measureFrustum(const FrustumCorners& frustumCorners)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float3 center{0.0f};
            for (const auto& p: frustumCorners)
                center += p;
            center /= frustumCorners.size();

            auto radius = 0.0f;
            for (const auto& p: frustumCorners) {
                const auto distance = length(p - center);
                radius              = max(radius, distance);
            }
            radius = ceil(radius * 16.0f) / 16.0f;
            return std::make_tuple(center, radius);
        }

        void RZCSMPass::eliminateShimmering(float4x4& projection, const float4x4& view, u32 shadowMapSize)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto shadowOrigin = projection * view * float4{float3{0.0f}, 1.0f};
            shadowOrigin *= (static_cast<f32>(shadowMapSize) / 2.0f);

            const auto roundedOrigin = round(shadowOrigin);
            auto       roundOffset   = roundedOrigin - shadowOrigin;
            roundOffset              = roundOffset * 2.0f / static_cast<f32>(shadowMapSize);
            roundOffset.z            = 0.0f;
            roundOffset.w            = 0.0f;
            projection[3] += roundOffset;
        }

        float4x4 RZCSMPass::buildDirLightMatrix(const float4x4& inversedViewProj, const float3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            const auto frustumCorners   = buildFrustumCorners(inversedViewProj, splitDist, lastSplitDist);
            const auto [center, radius] = measureFrustum(frustumCorners);

            const auto maxExtents = float3{radius};
            const auto minExtents = -maxExtents;

            const auto eye        = center - normalize(lightDirection) * -minExtents.z;
            const auto view       = lookAt(eye, center, {0.0f, 1.0f, 0.0f});
            auto       projection = ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -(maxExtents - minExtents).z, (maxExtents - minExtents).z);
            //projection            = float4x4(1.0f);
            //if (maxExtents.z != 0)
            //    projection = perspective(60.0f, (maxExtents.x - minExtents.x) / (maxExtents.y - minExtents.y), minExtents.z, maxExtents.z);

            if (Gfx::RZGraphicsContext::GetRenderAPI() == Gfx::RenderAPI::VULKAN)
                projection[1][1] *= -1;

            eliminateShimmering(projection, view, shadowMapSize);
            return projection * view;
        }

        struct LightVPLayerData
        {
            float4x4 viewProj = {};
            i32       layer    = 0;
        };

        CascadeSubPassData RZCSMPass::addCascadePass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings, CascadeSubPassData subpassData, u32 cascadeIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::CSM);

            auto& pass = framegraph.addCallbackPass<CascadeSubPassData>(
                "Pass.Builtin.Code.CSM # " + std::to_string(cascadeIdx),
                [&](CascadeSubPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // Create the resource only on first pass, render to the same resource again and again
                    if (cascadeIdx == 0) {
                        RZTextureDesc textureDesc{};
                        textureDesc.name   = "CascadedShadowMapArray";
                        textureDesc.width  = kShadowMapSize;
                        textureDesc.height = kShadowMapSize;
                        textureDesc.layers = kNumCascades;
                        textureDesc.type   = TextureType::k2DArray;
                        textureDesc.format = TextureFormat::DEPTH32F;

                        subpassData.cascadeOutput = builder.create<FrameGraph::RZFrameGraphTexture>("CascadedShadowMapArray", CAST_TO_FG_TEX_DESC textureDesc);
                    }

                    RZBufferDesc bufferDesc = {};
                    bufferDesc.name         = "VPLayer";
                    bufferDesc.size         = sizeof(LightVPLayerData);
                    bufferDesc.data         = nullptr;
                    bufferDesc.usage        = BufferUsage::PersistentStream;

                    subpassData.vpLayer = builder.create<FrameGraph::RZFrameGraphBuffer>("VPLayer", CAST_TO_FG_BUF_DESC bufferDesc);

                    data.cascadeOutput = builder.write(subpassData.cascadeOutput);
                    data.vpLayer       = builder.write(subpassData.vpLayer);
                },
                [=](const CascadeSubPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_Shadows);

                    RAZIX_TIME_STAMP_BEGIN("CSM Pass # " + std::to_string(cascadeIdx));
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.CSM # " + std::to_string(cascadeIdx), float4(0.35, 0.44, 0.96f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        auto descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(data.vpLayer)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.vpLayer).getHandle();

                        RZDescriptorSetDesc setDesc = {};
                        setDesc.name                = "CSM VPLayer # " + std::to_string(cascadeIdx);
                        setDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                        setDesc.descriptors.push_back(*descriptor);
                        m_CascadeSets[cascadeIdx] = RZResourceManager::Get().createDescriptorSet(setDesc);
                    }

                    auto lightVPHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.vpLayer).getHandle();

                    LightVPLayerData lightVPData{};
                    lightVPData.layer    = cascadeIdx;
                    lightVPData.viewProj = m_Cascades[cascadeIdx].viewProjMatrix;

                    RZResourceManager::Get().getUniformBufferResource(lightVPHandle)->SetData(sizeof(LightVPLayerData), &lightVPData);

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.resolution      = Resolution::kCustom;
                    m_CSMArrayHandle     = resources.get<FrameGraph::RZFrameGraphTexture>(data.cascadeOutput).getHandle();
                    info.depthAttachment = {m_CSMArrayHandle, {cascadeIdx == 0 ? true : false, ClearColorPresets::DepthOneToZero}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    /////////////////////////////////
                    // !!! VERY IMPORTANT !!!
                    info.layerCount = kNumCascades;
                    /////////////////////////////////
                    RHI::BeginRendering(cmdBuffer, info);

                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    auto shaderResource  = Gfx::RZResourceManager::Get().getShaderResource(shader);
                    auto sceneDrawParams = shaderResource->getSceneDrawParams();
                    sceneDrawParams.userSets.clear();
                    sceneDrawParams.userSets.push_back(m_CascadeSets[cascadeIdx]);
                    shaderResource->overrideSceneDrawParams(sceneDrawParams);

                    // Draw calls
                    // Get the meshes from the Scene and render them
                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    // End Rendering
                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();

                    // Schedule buffer update outside a render pass (force)
                });

            return pass;
        }
    }    // namespace Gfx
}    // namespace Razix
