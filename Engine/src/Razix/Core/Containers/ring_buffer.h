#ifndef _RZ_RINGBUFFER_H_
#define _RZ_RINGBUFFER_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/utility.h"

#define RZ_DEFAULT_QUEUE_CAPACITY 64

namespace Razix {

    template<typename T>
    class RZRingBuffer final
    {
    public:
        explicit RZRingBuffer(sz initialCapacity = RZ_DEFAULT_QUEUE_CAPACITY);
        ~RZRingBuffer();

        RAZIX_NONCOPYABLE_CLASS(RZRingBuffer);

        RZRingBuffer(RZRingBuffer&& other) noexcept;
        RZRingBuffer& operator=(RZRingBuffer&& other) noexcept;

        template<typename... Args>
        void emplace(Args&&... args);
        void write(const T& val);
        void write(T&& val);
        T    read();

        T&       head();
        const T& head() const;
        T&       tail();
        const T& tail() const;

        void clear();
        sz   size() const;
        sz   capacity() const;
        bool empty() const;
        bool isFull() const;
        void reserve(sz capacity);

        T*       begin();
        T*       end();
        const T* begin() const;
        const T* end() const;

        void destroy();

    private:
        T* m_data     = nullptr;
        sz m_capacity = 0;
        sz m_write    = 0;
        sz m_read     = 0;

        T*       ptr_at(size_t index) noexcept { return reinterpret_cast<T*>(reinterpret_cast<char*>(m_data) + index * sizeof(T)); }
        const T* ptr_at(size_t index) const noexcept { return reinterpret_cast<const T*>(reinterpret_cast<const char*>(m_data) + index * sizeof(T)); }
    };

    // ---------------- Implementation ----------------

    template<typename T>
    RZRingBuffer<T>::RZRingBuffer(sz initialCapacity)
    {
        reserve(initialCapacity);
    }

    template<typename T>
    RZRingBuffer<T>::~RZRingBuffer()
    {
        clear();
        destroy();
    }

    template<typename T>
    RZRingBuffer<T>::RZRingBuffer(RZRingBuffer&& other) noexcept
        : m_data(other.m_data), m_capacity(other.m_capacity), m_write(other.m_write), m_read(other.m_read)
    {
        other.m_data     = NULL;
        other.m_capacity = 0;
        other.m_write    = 0;
        other.m_read     = 0;
    }

    template<typename T>
    RZRingBuffer<T>& RZRingBuffer<T>::operator=(RZRingBuffer&& other) noexcept
    {
        if (this != &other) {
            destroy();

            m_data     = other.m_data;
            m_capacity = other.m_capacity;
            m_write    = other.m_write;
            m_read     = other.m_read;

            other.m_data     = NULL;
            other.m_capacity = 0;
            other.m_write    = 0;
            other.m_read     = 0;
        }
        return *this;
    }

    template<typename T>
    void RZRingBuffer<T>::reserve(sz capacity)
    {
        if (capacity <= m_capacity && m_data) return;

        T* new_data = static_cast<T*>(Memory::RZMalloc(capacity * sizeof(T), 16));
        if (!new_data) {
            RAZIX_CORE_ERROR("[RZRingBuffer] Failed to allocate memory for RZRingBuffer");
            return;
        }

        m_data     = new_data;
        m_capacity = capacity;
        m_read     = 0;
        m_write    = 0;
    }

    template<typename T>
    void RZRingBuffer<T>::destroy()
    {
        if (m_data) {
            Memory::RZFree(m_data);
            m_data     = nullptr;
            m_capacity = 0;
            m_write    = 0;
            m_read     = 0;
        }
    }

    template<typename T>
    bool RZRingBuffer<T>::isFull() const
    {
        return ((m_write + 1) % m_capacity) == m_read;
    }

    template<typename T>
    bool RZRingBuffer<T>::empty() const
    {
        return m_write == m_read;
    }

    template<typename T>
    sz RZRingBuffer<T>::size() const
    {
        return (m_write - m_read + m_capacity) % m_capacity;
    }

    template<typename T>
    sz RZRingBuffer<T>::capacity() const
    {
        return m_capacity;
    }

    template<typename T>
    void RZRingBuffer<T>::clear()
    {
        m_read  = 0;
        m_write = 0;
    }

    template<typename T>
    T& RZRingBuffer<T>::head()
    {
        sz idx = m_read % m_capacity;
        return *ptr_at(idx);
    }

    template<typename T>
    const T& RZRingBuffer<T>::head() const
    {
        sz idx = m_read % m_capacity;
        return *ptr_at(idx);
    }

    template<typename T>
    T& RZRingBuffer<T>::tail()
    {
        sz idx = (m_write + m_capacity - 1) % m_capacity;
        return *ptr_at(idx);
    }

    template<typename T>
    const T& RZRingBuffer<T>::tail() const
    {
        sz idx = (m_write + m_capacity - 1) % m_capacity;
        return *ptr_at(idx);
    }

    template<typename T>
    template<typename... Args>
    void RZRingBuffer<T>::emplace(Args&&... args)
    {
        sz idx = m_write % m_capacity;
        new (ptr_at(idx)) T(std::forward<Args>(args)...);
        m_write = (m_write + 1) % m_capacity;
    }

    template<typename T>
    void RZRingBuffer<T>::write(const T& val)
    {
        sz idx = m_write % m_capacity;
        new (ptr_at(idx)) T(val);
        m_write = (m_write + 1) % m_capacity;
    }

    template<typename T>
    void RZRingBuffer<T>::write(T&& val)
    {
        sz idx = m_write % m_capacity;
        new (ptr_at(idx)) T(rz_move(val));
        m_write = (m_write + 1) % m_capacity;
    }

    template<typename T>
    T RZRingBuffer<T>::read()
    {
        sz idx = m_read % m_capacity;
        T  val = rz_move(*ptr_at(idx));
        m_read = (m_read + 1) % m_capacity;
        return val;
    }

    template<typename T>
    T* RZRingBuffer<T>::begin()
    {
        return ptr_at(m_read);
    }

    template<typename T>
    T* RZRingBuffer<T>::end()
    {
        return ptr_at(m_write);
    }

    template<typename T>
    const T* RZRingBuffer<T>::begin() const
    {
        return ptr_at(m_read);
    }

    template<typename T>
    const T* RZRingBuffer<T>::end() const
    {
        return ptr_at(m_write);
    }

}    // namespace Razix

#endif    // _RZ_RINGBUFFER_H_