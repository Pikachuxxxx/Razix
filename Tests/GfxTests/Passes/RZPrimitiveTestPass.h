#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        // Primitive test pass: draws instanced primitives (e.g., cubes) using a VS+PS pipeline.
        // Focus: demonstrating vertex/index buffers, instance transform buffer, indirect draw args scaffolding.
        class RZPrimitiveTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            rz_gfx_shader_handle         m_Shader             = {};
            rz_gfx_pipeline_handle       m_Pipeline           = {};
            rz_gfx_root_signature_handle m_RootSigHandle      = {};
            rz_gfx_buffer_handle         m_VertexBuffer       = {};
            rz_gfx_buffer_handle         m_IndexBuffer        = {};
            rz_gfx_buffer_handle         m_IndirectArgsBuffer = {};

            u32 m_NumVertices = 0;
            u32 m_NumIndices  = 0;

            void createGeometryCPU();        // Fills CPU side arrays & uploads to GPU buffers (scaffolding)
            void createInstanceDataCPU();    // Generates transforms + uploads
            void createIndirectArgs();       // Populates indirect draw args buffer (placeholder; actual drawIndirect API TODO)
        };
    }    // namespace Gfx
}    // namespace Razix
