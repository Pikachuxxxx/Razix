#if 0
#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;
        class RZUniformBuffer;
        class RZVertexBuffer;
        class RZIndexBuffer;

        /**
         * This renderer is used to draw the Grid for editing and reference purposes which scales with the camera
         * Note:- This renderer doesn't need a material
         */
        class RAZIX_API RZGridRenderer : public IRZRenderer
        {
        public:
            struct GridUBOData
            {
                glm::vec3 cameraPos   = glm::vec3(0.0f);
                f32     _padding    = 0.0f;
                f32     scale       = 1000.0f;
                f32     resolution  = 2.25f;
                f32     maxDistance = 200.0f;
                f32     _padding2   = 0.0f;
            };

        public:
            RZGridRenderer() {}
            ~RZGridRenderer() {}

            void Init() override;

            void InitDisposableResources() override;

            void Begin() override;

            void BeginScene(Razix::RZScene* scene) override;

            void Submit(RZCommandBuffer* cmdBuf) override;

            void EndScene(Razix::RZScene* scene) override;

            void End() override;

            void Present() override;

            void Resize(u32 width, u32 height) override;

             void Destroy() override;

            void OnEvent(RZEvent& event) override;

        private:
            RZUniformBuffer*            m_GridUBO                 = nullptr;
            u8                     _padding[8];
            GridUBOData                 m_GridUBOData;
            Graphics::RZVertexBuffer*   gridVBO;
            Graphics::RZIndexBuffer*    gridIBO;

        };
    }    // namespace Graphics
}    // namespace Razix
#endif
