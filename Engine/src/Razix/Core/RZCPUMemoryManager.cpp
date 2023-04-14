// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCPUMemoryManager.h"

namespace Razix {

    void RZCPUMemoryManager::Init(u32 systemHeapSize)
    {
        m_SystemAllocator.init(systemHeapSize);

        m_FrameAllocator.init(RAZIX_FRAME_BUDGET);

        m_PacketAllocator.init(RAZIX_PACKETS * RAZIX_PACKET_SIZE);
    }

    void RZCPUMemoryManager::Shutdown()
    {
        m_SystemAllocator.shutdown();
        m_FrameAllocator.shutdown();
        m_PacketAllocator.shutdown();
    }
}    // namespace Razix