// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShadowRenderer.h"

#include "Razix/Graphics/API/RZUniformBuffer.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZShadowRenderer::Init()
        {
            // Init the resources (API & Frame Graph) needed for Cascaded Shadow Mapping
            // Import the ShadowMapData as a Buffer into the FrameGraph
            m_CascadedMatricesUBO = Graphics::RZUniformBuffer::Create(sizeof(CasdacesUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascades UBO"));
        }

        void RZShadowRenderer::Begin(RZScene* scene)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZShadowRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZShadowRenderer::End()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZShadowRenderer::Resize(uint32_t width, uint32_t height)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZShadowRenderer::Destroy()
        {
            m_CascadedMatricesUBO->Destroy();
        }

        //--------------------------------------------------------------------------

        void RZShadowRenderer::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto& shadowMapData = blackboard.add<ShadowMapData>();
            // Import the cascades buffer into the FG
            shadowMapData.viewProjMatrices = framegraph.import <FrameGraph::RZFrameGraphBuffer>("Cascade Matrices", {"Cascades UBO", sizeof(CasdacesUBOData)}, {m_CascadedMatricesUBO});

            // Build Cascades
            auto cascades = buildCascades(scene->getSceneCamera(), glm::vec3(1.0f), kNumCascades, 0.94f, kShadowMapSize);

            FrameGraph::RZFrameGraphResource cascaseShadowMaps{-1};
            for (uint32_t i = 0; i < cascades.size(); i++) {
                const glm::mat4& lightViewProj = cascades[i].viewProjMatrix;
                cascaseShadowMaps              = addCascadePass(framegraph, cascaseShadowMaps, lightViewProj, scene, i);
            }

            shadowMapData.cascadedShadowMaps = cascaseShadowMaps;
        }

        void RZShadowRenderer::destoy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        //--------------------------------------------------------------------------

        std::vector<Cascade> RZShadowRenderer::buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, uint32_t numCascades, float lambda, uint32_t shadowMapSize)
        {
            // [Reference] https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/

            const float clipRange = camera.getPerspectiveFarClip() - camera.getPerspectiveNearClip();
            // Get the cascade splits
            const auto cascadeSplits = buildCascadeSplits(numCascades, lambda, camera.getPerspectiveNearClip(), clipRange);
            const auto invViewProj   = glm::inverse(camera.getProjection() * camera.getViewMatrix());

            auto lastSplitDist = 0.0f;

            std::vector<Cascade> cascades(numCascades);
            for (uint32_t i{0}; i < cascades.size(); ++i) {
                const auto splitDist = cascadeSplits[i];

                cascades[i] = {
                    (camera.getPerspectiveNearClip() + splitDist * clipRange) * -1.0f,
                    buildDirLightMatrix(invViewProj, dirLightDirection, shadowMapSize, splitDist, lastSplitDist),
                };
                lastSplitDist = splitDist;
            }
            return cascades;
        }

        std::vector<float> RZShadowRenderer::buildCascadeSplits(uint32_t numCascades, float lambda, float nearPlane, float clipRange)
        {
            constexpr auto kMinDistance = 0.0f, kMaxDistance = 1.0f;

            const auto minZ = nearPlane + kMinDistance * clipRange;
            const auto maxZ = nearPlane + kMaxDistance * clipRange;

            const auto range = maxZ - minZ;
            const auto ratio = maxZ / minZ;

            std::vector<float> cascadeSplits(numCascades);
            // Calculate split depths based on view camera frustum
            // Based on method presented in:
            // https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
            for (uint32_t i{0}; i < cascadeSplits.size(); ++i) {
                const auto p       = static_cast<float>(i + 1) / cascadeSplits.size();
                const auto log     = minZ * std::pow(ratio, p);
                const auto uniform = minZ + range * p;
                const auto d       = lambda * (log - uniform) + uniform;
                cascadeSplits[i]   = (d - nearPlane) / clipRange;
            }
            return cascadeSplits;
        }

        //--------------------------------------------------------------------------

        FrustumCorners RZShadowRenderer::buildFrustumCorners(const glm::mat4& inversedViewProj, float splitDist, float lastSplitDist)
        {
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
            for (uint32_t i{0}; i < 4; ++i) {
                const auto cornerRay     = frustumCorners[i + 4] - frustumCorners[i];
                const auto nearCornerRay = cornerRay * lastSplitDist;
                const auto farCornerRay  = cornerRay * splitDist;
                frustumCorners[i + 4]    = frustumCorners[i] + farCornerRay;
                frustumCorners[i]        = frustumCorners[i] + nearCornerRay;
            }
            return frustumCorners;
        }

        auto RZShadowRenderer::measureFrustum(const FrustumCorners& frustumCorners)
        {
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

        void RZShadowRenderer::eliminateShimmering(glm::mat4& projection, const glm::mat4& view, uint32_t shadowMapSize)
        {
            auto shadowOrigin = projection * view * glm::vec4{glm::vec3{0.0f}, 1.0f};
            shadowOrigin *= (static_cast<float>(shadowMapSize) / 2.0f);

            const auto roundedOrigin = glm::round(shadowOrigin);
            auto       roundOffset   = roundedOrigin - shadowOrigin;
            roundOffset              = roundOffset * 2.0f / static_cast<float>(shadowMapSize);
            roundOffset.z            = 0.0f;
            roundOffset.w            = 0.0f;
            projection[3] += roundOffset;
        }

        glm::mat4 RZShadowRenderer::buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, uint32_t shadowMapSize, float splitDist, float lastSplitDist)
        {
            const auto frustumCorners   = buildFrustumCorners(inversedViewProj, splitDist, lastSplitDist);
            const auto [center, radius] = measureFrustum(frustumCorners);

            const auto maxExtents = glm::vec3{radius};
            const auto minExtents = -maxExtents;

            const auto eye        = center - glm::normalize(lightDirection) * -minExtents.z;
            const auto view       = glm::lookAt(eye, center, {0.0f, 1.0f, 0.0f});
            auto       projection = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, (maxExtents - minExtents).z);
            eliminateShimmering(projection, view, shadowMapSize);
            return projection * view;
        }

        FrameGraph::RZFrameGraphResource RZShadowRenderer::addCascadePass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZFrameGraphResource cascadeShadowMap, const glm::mat4& lightViewProj, Razix::RZScene* scene, uint32_t cascadeIdx)
        {
            const auto name = "CSM #" + std::to_string(cascadeIdx);

            struct CascadeSubPassData
            {
                FrameGraph::RZFrameGraphResource cascadeOuput;
            };

            auto& pass = framegraph.addCallbackPass<CascadeSubPassData>(
                name,
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, CascadeSubPassData& data) {
                    if (cascadeIdx == 0) {
                        cascadeShadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("CascadedShadowMaps:" + std::to_string(cascadeIdx), {FrameGraph::TextureType::Texture_RenderTarget, "CascadedShadowMaps:" + std::to_string(cascadeIdx), {kShadowMapSize, kShadowMapSize}, RZTexture::Format::DEPTH32});
                    }
                    data.cascadeOuput = builder.write(cascadeShadowMap); },
                [=](const CascadeSubPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    constexpr float kFarPlane{1.0f};
                });

            return pass.cascadeOuput;
        }
    }    // namespace Graphics
}    // namespace Razix
