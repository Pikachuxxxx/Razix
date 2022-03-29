#pragma once

#include "Razix/Core/RZSmartPointers.h"

#include "Razix/Core/OS/RZWindow.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_METAL

#include <Metal/Metal.hpp>

namespace Razix {
    namespace Graphics{
    
        class MTLContext : public RZGraphicsContext
        {
        public:
            MTLContext(RZWindow* windowHandle);

            void Init() override;
            void Destroy() override;
            void ClearWithColor(float r, float g, float b) override {}
            
        private:
            RZWindow* m_Window; /* The window handle */
        };

    }
}

#endif
