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

            void Resize(u32 width, u32 height) override;

            void Destroy() override;

            virtual void SetFrameDataHeap(RZDescriptorSet* frameDataSet) { m_FrameDataSet = frameDataSet; }

            void setCSMArrayHeap(RZDescriptorSet* csmSet) { m_CSMSet = csmSet; }

        private:
            RZUniformBuffer* m_ForwardLightsUBO        = nullptr;
            RZDescriptorSet* m_GPULightsDescriptorSet  = nullptr;
            RZScene*         m_CurrentScene            = nullptr;
            RZDescriptorSet* m_FrameDataSet            = nullptr;
            RZDescriptorSet* m_CSMSet                  = nullptr;
            RZShader*        m_OverrideGlobalRHIShader = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix