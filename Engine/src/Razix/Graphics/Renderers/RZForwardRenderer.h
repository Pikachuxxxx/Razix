#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;
        class RZUniformBuffer;
        class RZVertexBuffer;
        class RZIndexBuffer;

        class RAZIX_API RZForwardRenderer : public IRZRenderer
        {
        public:
            RZForwardRenderer() {}
            ~RZForwardRenderer() {}

            //--------------------------------------------------------------------------
            // IRZRenderer
            void Init() override;

            void Begin(RZScene* scene) override;

            void Draw(RZCommandBuffer* cmdBuffer) override;

            void End() override;

            void Resize(uint32_t width, uint32_t height) override;

            void Destroy() override;

        private:
            GPULightsData                    gpuLightsData{};
            RZUniformBuffer*                 m_ForwardLightsUBO = nullptr;
            ModelViewProjectionSystemUBOData mvpData{};
            RZUniformBuffer*                 m_SystemMVPUBO           = nullptr;
            RZDescriptorSet*                 m_GPULightsDescriptorSet = nullptr;
            RZScene*                         m_CurrentScene           = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix