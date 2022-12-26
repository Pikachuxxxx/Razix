#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/Passes/Data/ShadowMapData.h"

namespace Razix {

    class RZTimestep;

    namespace Graphics {

        class RZCommandBuffer;

        using FrustumCorners = std::array<glm::vec3, 8>;

        class RZShadowRenderer : public IRZRenderer, IRZPass
        {
        public:
            RZShadowRenderer()  = default;
            ~RZShadowRenderer() = default;

        public:
            //--------------------------------------------------------------------------
            // IRZRenderer
            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(uint32_t width, uint32_t height) override;

            void Destroy() override;

            //--------------------------------------------------------------------------
            // IRZPass

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override {}

            //--------------------------------------------------------------------------

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
            static std::vector<Cascade> buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, uint32_t numCascades, float lambda, uint32_t shadowMapSize);
            static std::vector<float>   buildCascadeSplits(uint32_t numCascades, float lambda, float nearPlane, float clipRange);

            static FrustumCorners buildFrustumCorners(const glm::mat4& inversedViewProj, float splitDist, float lastSplitDist);
            static auto           measureFrustum(const FrustumCorners& frustumCorners);
            static void           eliminateShimmering(glm::mat4& projection, const glm::mat4& view, uint32_t shadowMapSize);
            static glm::mat4      buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, uint32_t shadowMapSize, float splitDist, float lastSplitDist);

        private:
            RZUniformBuffer* m_CascadedMatricesUBO;
            struct CascadeGPUResources
            {
                std::vector<RZCommandBuffer*> CmdBuffers;
                RZUniformBuffer*              ViewProjLayerUBO;
                std::vector<RZDescriptorSet*> CascadeVPSet;
                RZPipeline*                   CascadePassPipeline;
            } cascadeGPUResources[kNumCascades];

            FrameGraph::RZFrameGraphResource addCascadePass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZFrameGraphResource cascadeShadowMap, const glm::mat4& lightViewProj, Razix::RZScene* scene, uint32_t cascadeIdx);
        };
    }    // namespace Graphics
}    // namespace Razix
