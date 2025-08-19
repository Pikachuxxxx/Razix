#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZHelloTextureTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            rz_gfx_texture_handle        m_TestTextureHandle;
            rz_gfx_resource_view_handle  m_TestTextureViewHandle;
            rz_gfx_shader_handle         m_Shader;
            rz_gfx_pipeline_handle       m_Pipeline;
            rz_gfx_root_signature_handle m_RootSigHandle;
        };
    }    // namespace Gfx
}    // namespace Razix
