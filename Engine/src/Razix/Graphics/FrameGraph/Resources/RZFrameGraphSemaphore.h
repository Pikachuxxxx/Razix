#pragma once

#include "Razix/Graphics/API/RZTexture.h"
#include "razix\Graphics\API\RZSemaphore.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {
            class RZFrameGraphSemaphore
            {
            public:
                struct Desc
                {
                    std::string name;
                };

                void create(const Desc& desc, void* allocator);
                void destroy(const Desc& desc, void* allocator);

                static std::string toString(const Desc& desc) { return "Semaphore"; }

                Graphics::RZSemaphore* getHandle() { return m_SemaphoreHandle; }

            private:
                Graphics::RZSemaphore* m_SemaphoreHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix