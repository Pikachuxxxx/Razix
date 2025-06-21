#pragma once

#include "internal/RazixMemory/include/RZAllocators.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {

#define RAZIX_PACKET_SIZE Kib(64)    // Max size of each packet in a 2-sided allocator
#define RAZIX_PACKETS     1024       // Total number of two-sided allocations

    /**
     * One stop shop for Managing CPU memory using various allocators
     * 
     * The Razix::Memory module is used to build and manage different allocators for 
     * different resources and assets from here, this is where we configure the memory 
     * allocators and tracking metrics and settings from for the entire engine.
     */
    class RZCPUMemoryManager final : public RZSingleton<RZCPUMemoryManager>
    {
    public:
        void Init(u32 systemHeapSize);
        void Release();

        inline Razix::Memory::RZHeapAllocator&     getSystemAllocator() { return m_SystemAllocator; }
        inline Razix::Memory::RZLinearAllocator&   getFrameAllocator() { return m_FrameAllocator; }
        inline Razix::Memory::RZTwoSidedAllocator& getPacketAllocator() { return m_PacketAllocator; }

    private:
        Razix::Memory::RZHeapAllocator     m_SystemAllocator;
        Razix::Memory::RZLinearAllocator   m_FrameAllocator;
        Razix::Memory::RZTwoSidedAllocator m_PacketAllocator;
    };
}    // namespace Razix
