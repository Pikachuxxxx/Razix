#include "RZBlitToSwapchainPass.h"

namespace Razix {
    namespace Gfx {

        void RZBlitToSwapchainPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            struct PassData
            {
                RZFrameGraphResource Src;
            };

            framegraph.addCallbackPass<PassData>(
                "[Test] Pass.Builtin.Code.BlitToSwapchain",
                [&](PassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    rz_gfx_resource_view_desc srvView      = {};
                    srvView.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_TEXTURE;
                    srvView.textureViewDesc.pTexture       = RZ_FG_TEX_RES_AUTO_POPULATE;
                    srvView.textureViewDesc.baseMip        = 0;
                    srvView.textureViewDesc.baseArrayLayer = 0;
                    srvView.textureViewDesc.dimension      = 1;

                    if (m_BlitTexture == -1)
                        RAZIX_ERROR("Blit Texture not set for the BlitToSwapchain Pass, please use setBlitTexture() to pass the FrameGraphResource.");

                    data.Src = builder.read(m_BlitTexture, srvView);
                },
                [=](const PassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_TIME_STAMP_BEGIN("[Test] BlitToSwapchain Pass");

                    rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
                    RAZIX_MARK_BEGIN(cmdBuffer, "[Test] Pass.Builtin.Code.BlitToSwapchain", GenerateHashedColor4(222u));

                    rzRHI_CopyTextureToSwapchain(
                        cmdBuffer,
                        resources.get<RZFrameGraphTexture>(data.Src).getRHIHandle(),
                        RZEngine::Get().getWorldRenderer().getCurrSwapchainBackbufferPtr());

                    RAZIX_MARK_END(cmdBuffer);
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZBlitToSwapchainPass::destroy()
        { /* nothing to destroy */
        }
    }    // namespace Gfx
}    // namespace Razix
