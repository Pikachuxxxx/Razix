#pragma once

#include "Razix/Graphics/API/RZCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLCommandBuffer : public RZCommandBuffer
        {
        public:
            OpenGLCommandBuffer ();
            ~OpenGLCommandBuffer ();

            void Init () override;
            void BeginRecording () override;
            void EndRecording () override;
            void Execute () override;
            void Reset () override;

            //void Draw(uint32_t verticexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
            //void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

            void UpdateViewport (uint32_t width, uint32_t height) override;
        };
    }    // namespace Graphics
}    // namespace Razix