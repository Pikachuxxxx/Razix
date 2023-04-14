#pragma once

#include "internal/RazixMemory/include/Allocators/RZHeapAllocator.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Graphics {

        /**
         * Allocated and Manages GPU Video Memory
         */
        class RZGPUMemoryManager : public RZSingleton<RZGPUMemoryManager>
        {
        public:
            void Init(u32 size);
            void ShutDown();

            RAZIX_INLINE const Razix::Memory::RZHeapAllocator& getVideoAllocator() const { return m_VideoAllocator; }
        private:
            Razix::Memory::RZHeapAllocator m_VideoAllocator;
            //Razix::Memory::RZRingAllocator m_VideoRingAllocator;
        };
    }    // namespace Graphics
}    // namespace Razix
