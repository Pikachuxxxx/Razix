#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        struct BloomPassData
        {
            FrameGraph::RZFrameGraphResource bloomTexture;
        };

        struct BloomMip
        {
            FrameGraph::RZFrameGraphResource mip{-1};
            glm::vec2                        size;
        };

        constexpr u32 NUM_BLOOM_MIPS = 5;

        class RZBloomPass : public IRZPass
        {
        public:
            RZBloomPass() {}
            ~RZBloomPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;

            void destroy() override;

        private:
            RZPipeline* m_UpsamplePipeline;
            RZPipeline* m_DownsamplePipeline;
            struct BloomPassGPUResources
            {
                std::vector<RZCommandBuffer*> cmdBuffers;
                std::vector<RZDescriptorSet*> bloomDescSet;
            };
            BloomPassGPUResources downsamplebBloomGpuResources[NUM_BLOOM_MIPS];
            BloomPassGPUResources upsamplebBloomGpuResources[NUM_BLOOM_MIPS];
            RZMesh*               m_ScreenQuadMesh;

        private:
            BloomMip upsample(FrameGraph::RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex);
            BloomMip downsample(FrameGraph::RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex);
            void     mixScene(FrameGraph::RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene);
        };
    }    // namespace Graphics
}    // namespace Razix
