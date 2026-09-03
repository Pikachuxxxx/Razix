#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZHelloTriangleTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph) override;
            void destroy();

        private:
            rz_gfx_shader_handle         m_Shader;
            rz_gfx_pipeline_handle       m_Pipeline;
            rz_gfx_root_signature_handle m_RootSigHandle;
        };
    }    // namespace Gfx
}    // namespace Razix
