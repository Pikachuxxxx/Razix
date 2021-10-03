#pragma once

#include "Razix/Graphics/API/GraphicsContext.h"

namespace Razix {
    namespace Graphics {

        class DX11Context : public GraphicsContext
        {
        public:
            void Init() override;
            void SwapBuffers() override;
            void ClearWithColor(float r, float g, float b) override;

        };

    }
}