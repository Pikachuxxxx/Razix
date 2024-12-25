#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/Passes/Data/CSMData.h"

namespace Razix {
    class RZSceneCamera;
    namespace Gfx {

        class RZDescriptorSet;

        using FrustumCorners = std::array<glm::vec3, 8>;

        struct CascadeSubPassData
        {
            FrameGraph::RZFrameGraphResource cascadeOutput;
            FrameGraph::RZFrameGraphResource vpLayer;
        };

        class RZCSMPass : public IRZPass
        {
        public:
            RZCSMPass() {}
            ~RZCSMPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

            void updateCascades(Razix::RZScene* scene);

            RAZIX_INLINE RZTextureHandle getCSMArrayTex() const { return m_CSMArrayHandle; }
            RAZIX_INLINE const auto&     getCascades() const { return m_Cascades; }

            static std::vector<Cascade> buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize);
            static std::vector<f32>     buildCascadeSplits(u32 numCascades, f32 lambda, f32 nearPlane, f32 clipRange);
            static FrustumCorners       buildFrustumCorners(const glm::mat4& inversedViewProj, f32 splitDist, f32 lastSplitDist);
            static auto                 measureFrustum(const FrustumCorners& frustumCorners);
            static void                 eliminateShimmering(glm::mat4& projection, const glm::mat4& view, u32 shadowMapSize);
            static glm::mat4            buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist);

        private:
            RZPipelineHandle      m_Pipeline                  = {};
            std::vector<Cascade>  m_Cascades                  = {};
            RZDescriptorSetHandle m_CascadeSets[kNumCascades] = {};
            RZTextureHandle       m_CSMArrayHandle            = {};

        private:
            CascadeSubPassData addCascadePass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings, CascadeSubPassData subpassData, u32 cascadeIdx);
        };
    }    // namespace Gfx
}    // namespace Razix
