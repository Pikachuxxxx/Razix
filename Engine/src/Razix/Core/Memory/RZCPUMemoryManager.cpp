// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCPUMemoryManager.h"

#include "Razix/Core/Memory/RZMemoryBudgets.h"

namespace Razix {

    void RZCPUMemoryManager::Init(u32 systemHeapSize)
    {
        m_SystemAllocator.init(systemHeapSize);

        u32 cpuMemoryFrameBudget = Memory::GetGlobalFrameBudget().MemoryBudget;
        m_FrameAllocator.init(cpuMemoryFrameBudget);

        m_PacketAllocator.init(RAZIX_PACKETS * RAZIX_PACKET_SIZE);
    }

    void RZCPUMemoryManager::Release()
    {
        m_SystemAllocator.shutdown();
        m_FrameAllocator.shutdown();
        m_PacketAllocator.shutdown();
    }
}    // namespace Razix
