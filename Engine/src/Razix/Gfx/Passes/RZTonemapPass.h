#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZToneMapPass : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override {}

        private:
            rz_gfx_pipeline_handle       m_Pipeline;
            rz_gfx_root_signature_handle m_RootSignature;
        };
    }    // namespace Gfx
}    // namespace Razix
