#pragma once

#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLCommandBuffer : public RZDrawCommandBuffer
        {
        public:
            OpenGLCommandBuffer();
            ~OpenGLCommandBuffer();

            void Init(RZ_DEBUG_NAME_TAG_S_ARG) override;
            void BeginRecording() override;
            void EndRecording() override;
            void Execute() override;
            void Reset() override;
        };
    }    // namespace Graphics
}    // namespace Razix