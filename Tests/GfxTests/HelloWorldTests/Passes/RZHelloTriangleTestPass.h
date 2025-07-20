#pragma once

#include <Razix.h>

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZHelloTriangleTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            //RZPipelineHandle m_Pipeline;
            rz_gfx_shader_handle m_Shader;
        };
    }    // namespace Gfx
}    // namespace Razix
