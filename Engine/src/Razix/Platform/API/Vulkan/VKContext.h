#pragma once

#include "Razix/Graphics/API/GraphicsContext.h"

namespace Razix {
    namespace Graphics {

        class VKContext : public GraphicsContext
        {
        public:
            virtual void Init() override;
            virtual void SwapBuffers() override;
            void ClearWithColor(float r, float g, float b) override;

        };

    }
}
