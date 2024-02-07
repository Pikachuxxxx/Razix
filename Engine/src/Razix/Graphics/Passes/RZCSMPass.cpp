// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCSMPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {
    namespace Graphics {

        void RZCSMPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            /**
             * In CSM we render to a TextureArray Depth map, we select the render layer using gl_Layer or API (setCurrentArrayLayer), but the shaders remain the same as normal shadow map generation
             */
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::CSM);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Pipeline.CSM";
            pipelineInfo.cullMode            = Graphics::CullMode::Back;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.drawType            = Graphics::DrawType::Triangle;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthFormat         = {Graphics::TextureFormat::DEPTH32F};
            pipelineInfo.depthTestEnabled    = true;
            pipelineInfo.depthWriteEnabled   = true;
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();
            auto& csmData        = framegraph.getBlackboard().add<CSMData>();

            // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
            auto dirLight = scene->GetComponentsOfType<LightComponent>();
            m_Cascades    = buildCascades(scene->getSceneCamera(), dirLight[0].light.getPosition(), kNumCascades, 0.95f, kShadowMapSize);

            CascadeSubPassData cascadeSubpassData{-1};
            for (u32 i = 0; i < m_Cascades.size(); i++) {
                cascadeSubpassData = addCascadePass(framegraph, scene, cascadeSubpassData, i);
            }
            csmData.cascadedShadowMaps = cascadeSubpassData.cascadeOutput;

            // Since the above texture passes are cascaded we do an extra pass to constantly update the data into a buffer after all the cascade calculations are done whilst filling the TextureArray2D
            framegraph.addCallbackPass(
                "Upload Cascade Matrices (post CSM calculation)",
                [&](auto&, FrameGraph::RZPassResourceBuilder& builder) {
                    // Create the final cascaded VP data here after rendering the depth texture array, in fact this pass can be parallelized before the we render the depth maps
                    csmData.viewProjMatrices = builder.create<FrameGraph::RZFrameGraphBuffer>("CascadesMatrixData", {.name = "CascadesMatrixData", .size = sizeof(CascadesMatrixData), .data = nullptr, .usage = BufferUsage::PersistentStream});

                    csmData.viewProjMatrices = builder.write(csmData.viewProjMatrices);
                },
                [=](const auto&, FrameGraph::RZPassResourceDirectory& resources) {
                    CascadesMatrixData data{};
                    for (u32 i{0}; i < m_Cascades.size(); ++i) {
                        data.splitDepth[i]       = m_Cascades[i].splitDepth;
                        data.viewProjMatrices[i] = m_Cascades[i].viewProjMatrix;
                    }
                    auto vpbufferHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(csmData.viewProjMatrices).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(vpbufferHandle)->SetData(sizeof(CascadesMatrixData), &data);
                });
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
            m_Cascades    = buildCascades(scene->getSceneCamera(), dirLight[0].light.getPosition(), kNumCascades, 0.95f, kShadowMapSize);
        }

        std::vector<Cascade> RZCSMPass::buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // [Reference] https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/

            const f32 clipRange = camera.getPerspectiveFarClip() - camera.getPerspectiveNearClip();
            // Get the cascade splits
            const auto cascadeSplits  = buildCascadeSplits(numCascades, lambda, camera.getPerspectiveNearClip(), clipRange);
            const auto invViewProjRaw = camera.getProjectionRaw() * camera.getViewMatrix();
            const auto invViewProj    = (camera.getProjection() * camera.getViewMatrix());

            auto lastSplitDist = 0.0f;

            std::vector<Cascade> cascades(numCascades);
            for (u32 i{0}; i < cascades.size(); ++i) {
                const auto splitDist = cascadeSplits[i];

                cascades[i] = {
                    (camera.getPerspectiveNearClip() + splitDist * clipRange) * -1.0f,
                    buildDirLightMatrix(invViewProjRaw, dirLightDirection, shadowMapSize, splitDist, lastSplitDist),
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

        Razix::Graphics::FrustumCorners RZCSMPass::buildFrustumCorners(const glm::mat4& inversedViewProj, f32 splitDist, f32 lastSplitDist)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // clang-format off
            FrustumCorners frustumCorners{
                // Near
                glm::vec3{-1.0f,  1.0f, -1.0f}, // TL
                glm::vec3{ 1.0f,  1.0f, -1.0f}, // TR
                glm::vec3{ 1.0f, -1.0f, -1.0f}, // BR
                glm::vec3{-1.0f, -1.0f, -1.0f}, // BL
                // Far
                glm::vec3{-1.0f,  1.0f,  1.0f}, // TL
                glm::vec3{ 1.0f,  1.0f,  1.0f}, // TR
                glm::vec3{ 1.0f, -1.0f,  1.0f}, // BR
                glm::vec3{-1.0f, -1.0f,  1.0f}  // BL
            };
            // clang-format on

            // Project frustum corners into world space
            for (auto& p: frustumCorners) {
                const auto temp = inversedViewProj * glm::vec4{p, 1.0f};
                p               = glm::vec3{temp} / temp.w;
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

            glm::vec3 center{0.0f};
            for (const auto& p: frustumCorners)
                center += p;
            center /= frustumCorners.size();

            auto radius = 0.0f;
            for (const auto& p: frustumCorners) {
                const auto distance = glm::length(p - center);
                radius              = glm::max(radius, distance);
            }
            radius = glm::ceil(radius * 16.0f) / 16.0f;
            return std::make_tuple(center, radius);
        }

        void RZCSMPass::eliminateShimmering(glm::mat4& projection, const glm::mat4& view, u32 shadowMapSize)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto shadowOrigin = projection * view * glm::vec4{glm::vec3{0.0f}, 1.0f};
            shadowOrigin *= (static_cast<f32>(shadowMapSize) / 2.0f);

            const auto roundedOrigin = glm::round(shadowOrigin);
            auto       roundOffset   = roundedOrigin - shadowOrigin;
            roundOffset              = roundOffset * 2.0f / static_cast<f32>(shadowMapSize);
            roundOffset.z            = 0.0f;
            roundOffset.w            = 0.0f;
            projection[3] += roundOffset;
        }

        glm::mat4 RZCSMPass::buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            const auto frustumCorners   = buildFrustumCorners(inversedViewProj, splitDist, lastSplitDist);
            const auto [center, radius] = measureFrustum(frustumCorners);

            const auto maxExtents = glm::vec3{radius};
            const auto minExtents = -maxExtents;

            const auto eye        = center - glm::normalize(lightDirection) * -minExtents.z;
            const auto view       = glm::lookAt(eye, center, {0.0f, 1.0f, 0.0f});
            auto       projection = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, (maxExtents - minExtents).z);
            //if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::VULKAN)
            //    projection[1][1] *= -1;

            eliminateShimmering(projection, view, shadowMapSize);
            return projection * view;
        }

        struct LightVPLayerData
        {
            glm::mat4 viewProj = {};
            i32       layer    = 0;
        };

        CascadeSubPassData RZCSMPass::addCascadePass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, CascadeSubPassData subpassData, u32 cascadeIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& pass = framegraph.addCallbackPass<CascadeSubPassData>(
                "Pass.Builtin.Code.CSM # " + std::to_string(cascadeIdx),
                [&](CascadeSubPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // Create the resource only on first pass, render to the same resource again and again
                    if (cascadeIdx == 0) {
                        subpassData.cascadeOutput = builder.create<FrameGraph::RZFrameGraphTexture>("CascadedShadowMapArray", {.name = "CascadedShadowMapArray", .width = kShadowMapSize, .height = kShadowMapSize, .layers = kNumCascades, .type = TextureType::Texture_2DArray, .format = TextureFormat::DEPTH32F});
                        subpassData.vpLayer       = builder.create<FrameGraph::RZFrameGraphBuffer>("VPLayer", {.name = "VPLayer", .size = sizeof(LightVPLayerData), .data = nullptr, .usage = BufferUsage::PersistentStream});
                    }

                    data.cascadeOutput = builder.write(subpassData.cascadeOutput);
                    data.vpLayer       = builder.write(subpassData.vpLayer);
                },
                [=](const CascadeSubPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    //if (!(settings.renderFeatures & RendererFeature_Shadows))
                    //    return;

                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("CSM Pass # " + std::to_string(cascadeIdx));
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.CSM # " + std::to_string(cascadeIdx), glm::vec4(0.35, 0.44, 0.96f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto  shader         = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::CSM);
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        auto descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(data.vpLayer)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.vpLayer).getHandle();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    auto lightVPHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.vpLayer).getHandle();

                    LightVPLayerData lightVPData{};
                    lightVPData.layer    = cascadeIdx;
                    lightVPData.viewProj = m_Cascades[cascadeIdx].viewProjMatrix;
                    RZResourceManager::Get().getUniformBufferResource(lightVPHandle)->SetData(sizeof(LightVPLayerData), &lightVPData);

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.resolution      = Resolution::kCustom;
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.cascadeOutput).getHandle(), {cascadeIdx == 0 ? true : false, ClearColorPresets::DepthOneToZero}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    /////////////////////////////////
                    // !!! VERY IMPORTANT !!!
                    info.layerCount = kNumCascades;
                    /////////////////////////////////
                    info.resize = false;
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

            return pass;
        }
    }    // namespace Graphics
}    // namespace Razix