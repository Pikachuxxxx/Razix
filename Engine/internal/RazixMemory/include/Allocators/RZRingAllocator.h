#pragma once

#include "IRZAllocator.h"
#include "RZMemoryFunctions.h"

#include <cstdint>
#include <mutex>

namespace Razix {
    namespace Memory {

        template<class T>
        class RZRingAllocator : public IRZAllocator
        {
        public:
            RZRingAllocator()  = default;
            ~RZRingAllocator() = default;

            void init(size_t size) override;
            void shutdown() override;

            void* allocate(size_t size, size_t alignment) override;

            void deallocate(void* ptr) override;

            uint64_t getHead() { return m_Head; }
            uint64_t getTail() { return m_Tail; }
            void     reset();
            bool     isFull() { return m_IsFull; }
            bool     empty();

            void put(T item);
            T    get();

        private:
            std::mutex m_Mutex;             /* Mutex to make read/write from ring buffer thread safe */
            T*         m_Buffer  = nullptr; /* Raw data buffer of type T                             */
            uint64_t   m_Head    = 0;       /* Head of the ring buffer                               */
            uint64_t   m_Tail    = 0;       /* Tail of the ring buffer                               */
            uint32_t   m_MaxSize = 0;
            bool       m_IsFull  = false;
        };

        template<class T>
        void Razix::Memory::RZRingAllocator<T>::init(size_t size)
        {
            m_MaxSize = (uint32_t) size;
            m_Buffer  = (T*) RZMalloc(size);
        }

        template<class T>
        void Razix::Memory::RZRingAllocator<T>::deallocate(void* ptr)
        {
        }

        template<class T>
        void* Razix::Memory::RZRingAllocator<T>::allocate(size_t size, size_t alignment)
        {
            return m_Buffer;
        }

        template<class T>
        void Razix::Memory::RZRingAllocator<T>::shutdown()
        {
            reset();
            RZFree((void*) m_Buffer);
        }

        template<class T>
        void Razix::Memory::RZRingAllocator<T>::reset()
        {
            m_Head   = m_Tail;
            m_IsFull = false;
        }

        template<class T>
        bool Razix::Memory::RZRingAllocator<T>::empty()
        {
            //if head and tail are equal, we are empty
            return (!m_IsFull && (m_Head == m_Tail));
        }

        template<class T>
        void Razix::Memory::RZRingAllocator<T>::put(T item)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Buffer[m_Head] = item;

            if (m_IsFull) {
                m_Tail = (m_Tail + 1) % m_MaxSize;
            }

            m_Head = (m_Head + 1) % m_MaxSize;

            m_IsFull = m_Head == m_Tail;
        }

        template<class T>
        T Razix::Memory::RZRingAllocator<T>::get()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            if (empty())
                return T();

            //Read data and advance the tail (we now have a free space)
            auto value = m_Buffer[m_Tail];
            m_IsFull   = false;
            m_Tail     = (m_Tail + 1) % m_MaxSize;

            return value;
        }
    }    // namespace Memory
}    // namespace Razix