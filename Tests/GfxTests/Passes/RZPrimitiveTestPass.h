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
            // Pipeline + shader resources
            rz_gfx_shader_handle         m_Shader{};
            rz_gfx_pipeline_handle       m_Pipeline{};
            rz_gfx_root_signature_handle m_RootSigHandle{};

            // Geometry buffers (vertex, index, instance, indirect args)
            rz_gfx_buffer_handle m_VertexBuffer{};     // Per-vertex data (pos, normal, uv)
            rz_gfx_buffer_handle m_IndexBuffer{};      // Per-index (u32)
            rz_gfx_buffer_handle m_InstanceBuffer{};   // Mat4 transforms (or simplified) for instances
            rz_gfx_buffer_handle m_IndirectArgsBuffer{}; // rz_gfx_draw_indirect_args struct

            u32 m_NumVertices = 0; // Filled during setup
            u32 m_NumIndices  = 0; // Filled during setup
            u32 m_NumInstances = 0; // Filled during setup

            void createGeometryCPU(); // Fills CPU side arrays & uploads to GPU buffers (scaffolding)
            void createInstanceDataCPU(); // Generates transforms + uploads
            void createIndirectArgs(); // Populates indirect draw args buffer (placeholder; actual drawIndirect API TODO)
        };
    } // namespace Gfx
} // namespace Razix
