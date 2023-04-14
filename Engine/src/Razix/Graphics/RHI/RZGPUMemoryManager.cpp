// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGPUMemoryManager.h"

namespace Razix {
    namespace Graphics {

        void RZGPUMemoryManager::Init(u32 size)
        {
            m_VideoAllocator.init(size);
        }

        void RZGPUMemoryManager::ShutDown()
        {
            m_VideoAllocator.shutdown();
        }
    }    // namespace Graphics
}    // namespace Razix