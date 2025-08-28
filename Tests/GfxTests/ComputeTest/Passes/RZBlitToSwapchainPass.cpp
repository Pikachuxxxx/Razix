#include "RZBlitToSwapchainPass.h"

namespace Razix {
    namespace Gfx {

        struct MandleBrotBlackboardData
        {
            RZFrameGraphResource OutputTexture;
        };

        void RZBlitToSwapchainPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* /*scene*/, RZRendererSettings* /*settings*/)
        {
            // Acquire output texture from previous compute pass blackboard
            auto& mbData = framegraph.getBlackboard().get<MandleBrotBlackboardData>();

            struct PassData { RZFrameGraphResource Src; };

            framegraph.addCallbackPass<PassData>(
                "[Test] Pass.Builtin.Code.BlitToSwapchain",
                [&](PassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // Read (SRV) view of compute output
                    rz_gfx_resource_view_desc srvView = {};
                    srvView.descriptorType = RZ_GFX_DESCRIPTOR_TYPE_TEXTURE; // treat as sampled for blit or copy
                    srvView.textureViewDesc.pTexture = RZ_FG_TEX_RES_AUTO_POPULATE;
                    srvView.textureViewDesc.baseMip = 0;
                    srvView.textureViewDesc.baseArrayLayer = 0;
                    srvView.textureViewDesc.dimension = 1;

                    data.Src = builder.read(mbData.OutputTexture, srvView);
                },
                [=](const PassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] BlitToSwapchain Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.BlitToSwapchain", Utilities::GenerateHashedColor4(222u));

                    // Placeholder: perform blit from compute output to swapchain
                    // TODO: implement rzRHI_BlitTexture if not present.
                    // rzRHI_BlitTexture(cmdBuffer, resources.get<RZFrameGraphTexture>(data.Src).getRHIHandle(), RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferHandle());

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                }
            );
        }

        void RZBlitToSwapchainPass::destroy() { /* nothing to destroy */ }
    }
}
