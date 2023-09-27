#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/Passes/Data/ShadowMapData.h"

namespace Razix {

    class RZTimestep;

    namespace Graphics {

        class RZCommandBuffer;

        using FrustumCorners = std::array<glm::vec3, 8>;

        class RZCascadedShadowsRenderer : public IRZRenderer, IRZPass
        {
        public:
            RZCascadedShadowsRenderer()  = default;
            ~RZCascadedShadowsRenderer() = default;

        public:
            //--------------------------------------------------------------------------
            // IRZRenderer
            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(u32 width, u32 height) override;

            void Destroy() override;

            //--------------------------------------------------------------------------
            // IRZPass

            void addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override {}

            //--------------------------------------------------------------------------

            void                  updateCascades(RZScene* scene);
            RZUniformBufferHandle getCascadedMatriceUBO() { return m_CascadedMatricesUBO; }

            /**
             * Builds the cascaded shadow maps
             * 
             * @param camera The scene camera from which the scene is rendered 
             * @param dirLightDirection The Directional Light current direction
             * @param numCascades The total number of cascades to build
             * @param lambda IDK WTF is this
             * @param shadowMapSize The size of the shadow maps
             * 
             * @returns The split distance and the cascade view proj matrix
             */
            static std::vector<Cascade> buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize);
            static std::vector<f32>     buildCascadeSplits(u32 numCascades, f32 lambda, f32 nearPlane, f32 clipRange);

            static FrustumCorners buildFrustumCorners(const glm::mat4& inversedViewProj, f32 splitDist, f32 lastSplitDist);
            static auto           measureFrustum(const FrustumCorners& frustumCorners);
            static void           eliminateShimmering(glm::mat4& projection, const glm::mat4& view, u32 shadowMapSize);
            static glm::mat4      buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist);

        private:
            RZUniformBufferHandle m_CascadedMatricesUBO;
            struct CascadeGPUResources
            {
                std::vector<RZCommandBuffer*> CmdBuffers;
                RZUniformBufferHandle         ViewProjLayerUBO;
                std::vector<RZDescriptorSet*> CascadeVPSet;
                RZPipelineHandle              CascadePassPipeline;
            } cascadeGPUResources[kNumCascades];

            std::vector<Cascade> m_Cascades;

        private:
            FrameGraph::RZFrameGraphResource addCascadePass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZFrameGraphResource cascadeShadowMap, Razix::RZScene* scene, u32 cascadeIdx);
        };
    }    // namespace Graphics
}    // namespace Razix
