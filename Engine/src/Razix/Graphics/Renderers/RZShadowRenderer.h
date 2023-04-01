#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Renderers/IRZRenderer.h"

namespace Razix {
    namespace Graphics {

        struct LightVPUBOData
        {
            glm::mat4 lightViewProj;
        };

        struct SimpleShadowPassData
        {
            FrameGraph::RZFrameGraphResource shadowMap;    // Depth texture to store the shadow map data
            FrameGraph::RZFrameGraphResource lightVP;
        };

        struct OmniDirectionalShadowPassData
        {
            FrameGraph::RZFrameGraphResource shadowMap;       /* Cube Depth texture to store the shadow map data                    */
            FrameGraph::RZFrameGraphResource lightVPMatrices; /* The light view proj matrices to look at all sides of the cubemap   */
        };

        constexpr u32 kShadowMapSize = 4096;

        class RZShadowRenderer : public IRZRenderer, IRZPass
        {
        public:
            RZShadowRenderer()  = default;
            ~RZShadowRenderer() = default;

            //-------------------------------------------------------------
            // IRZRenderer

            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(u32 width, u32 height) override;

            void Destroy() override;

            void SetFrameDataHeap(RZDescriptorSet* frameDataSet) override;

            //-------------------------------------------------------------
            // IRZPass

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZUniformBuffer* m_LightViewProjUBO;
            RZDescriptorSet* m_FrameDataSet;
            RZDescriptorSet* m_LVPSet;
        };
    }    // namespace Graphics
}    // namespace Razix
