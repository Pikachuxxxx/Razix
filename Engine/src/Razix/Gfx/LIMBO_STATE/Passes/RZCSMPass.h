#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

namespace Razix {
    class RZSceneCamera;
    namespace Gfx {

        class RZDescriptorSet;

        using FrustumCorners = std::array<float3, 8>;

        struct CascadeSubPassData
        {
            RZFrameGraphResource cascadeOutput;
            RZFrameGraphResource vpLayer;
        };

        class RZCSMPass : public IRZPass
        {
        public:
            RZCSMPass() {}
            ~RZCSMPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

            void updateCascades(Razix::RZScene* scene);

            RAZIX_INLINE rz_texture_handle getCSMArrayTex() const { return m_CSMArrayHandle; }
            RAZIX_INLINE const auto&       getCascades() const { return m_Cascades; }

            static std::vector<Cascade> buildCascades(RZSceneCamera camera, float3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize);
            static std::vector<f32>     buildCascadeSplits(u32 numCascades, f32 lambda, f32 nearPlane, f32 clipRange);
            static FrustumCorners       buildFrustumCorners(const float4x4& inversedViewProj, f32 splitDist, f32 lastSplitDist);
            static auto                 measureFrustum(const FrustumCorners& frustumCorners);
            static void                 eliminateShimmering(float4x4& projection, const float4x4& view, u32 shadowMapSize);
            static float4x4             buildDirLightMatrix(const float4x4& inversedViewProj, const float3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist);

        private:
            RZPipelineHandle      m_Pipeline                  = {};
            std::vector<Cascade>  m_Cascades                  = {};
            RZDescriptorSetHandle m_CascadeSets[kNumCascades] = {};
            rz_texture_handle     m_CSMArrayHandle            = {};

        private:
            CascadeSubPassData addCascadePass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings, CascadeSubPassData subpassData, u32 cascadeIdx);
        };
    }    // namespace Gfx
}    // namespace Razix
