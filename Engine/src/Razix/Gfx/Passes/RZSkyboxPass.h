#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZSkyboxPass : public IRZPass
        {
        public:
            RZSkyboxPass() {}
            ~RZSkyboxPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            rz_gfx_pipeline_handle       m_SkyboxPipeline;
            rz_gfx_root_signature_handle m_SkyboxRootSigHandle;
            rz_gfx_shader_handle         m_SkyboxShader;
            rz_gfx_pipeline_handle       m_ProceduralPipeline;
            rz_gfx_root_signature_handle m_ProceduralRootSigHandle;
            rz_gfx_shader_handle         m_ProceduralSkyboxShader;
            rz_gfx_buffer_handle         m_VertexBuffer;
            rz_gfx_buffer_handle         m_IndexBuffer;
        };
    }    // namespace Gfx
}    // namespace Razix
