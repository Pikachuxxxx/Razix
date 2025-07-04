#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZMesh;

        struct BloomPassData
        {
            RZFrameGraphResource bloomTexture;
        };

        struct BloomMip
        {
            RZFrameGraphResource mip{-1};
            float2               size;
        };

        constexpr u32 NUM_BLOOM_MIPS = 5;

        class RZBloomPass : public IRZPass
        {
        public:
            RZBloomPass() {}
            ~RZBloomPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;

            void destroy() override;

        private:
            RZPipelineHandle m_UpsamplePipeline;
            RZPipelineHandle m_DownsamplePipeline;
            RZPipelineHandle m_HDRBloomMixPipeline;
            struct BloomPassGPUResources
            {
                std::vector<RZDrawCommandBuffer*> cmdBuffers;
                std::vector<RZDescriptorSet*>     bloomDescSet;
            };
            BloomPassGPUResources downsamplebBloomGpuResources[NUM_BLOOM_MIPS];
            BloomPassGPUResources upsamplebBloomGpuResources[NUM_BLOOM_MIPS];
            BloomPassGPUResources bloomSceneMixGpuResources;

            RZMesh* m_ScreenQuadMesh;

        private:
            BloomMip upsample(RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex, RZRendererSettings* settings);
            BloomMip downsample(RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex);
            /**
             * Mixes the final bloom blurred texture with the scene HDR color render target
             */
            void mixScene(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings);
        };
    }    // namespace Gfx
}    // namespace Razix
