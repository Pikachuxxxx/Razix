#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;
        class RZUniformBuffer;

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
                float     _padding    = 0.0f;
                float     scale       = 1.0f;
                float     resolution  = 1.4f;
                float     maxDistance = 600.0f;
                float     _padding2   = 0.0f;
            };

        public:
            RZGridRenderer() {}
            ~RZGridRenderer() {}

            void Init() override;

            void Begin() override;

            void BeginScene(RZScene* scene) override;

            void Submit(RZCommandBuffer& cmdBuf) override;

            void EndScene(RZScene* scene) override;

            void End() override;

            void Present() override;

            void Resize(uint32_t width, uint32_t height) override;

            void OnEvent(RZEvent& event) override;

        private:
            RZMesh*                     m_Plane                   = nullptr;
            RZUniformBuffer*            m_ViewProjectionSystemUBO = nullptr;
            RZUniformBuffer*            m_GridUBO                 = nullptr;
            uint8_t                     _padding[8];
            ViewProjectionSystemUBOData m_ViewProjSystemUBOData;
            GridUBOData                 m_GridUBOData;

        private:
            void InitDisposableResources();
        };
    }    // namespace Graphics
}    // namespace Razix
