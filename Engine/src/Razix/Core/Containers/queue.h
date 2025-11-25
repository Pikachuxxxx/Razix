#ifndef _RZ_QUEUE_H_
#define _RZ_QUEUE_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/utility.h"

#define RZ_DEFAULT_QUEUE_CAPACITY 64

namespace Razix {

    template<typename T>
    class RZQueue final
    {
    public:
        explicit RZQueue(sz initialCapacity = RZ_DEFAULT_QUEUE_CAPACITY);
        ~RZQueue();

        RAZIX_NONCOPYABLE_CLASS(RZQueue);

        RZQueue(RZQueue&& other) noexcept;
        RZQueue& operator=(RZQueue&& other) noexcept;

        template<typename... Args>
        void emplace(Args&&... args);
        void push(const T& val);
        void push(T&& val);
        T    pop();

        T&       front();
        const T& front() const;
        T&       back();
        const T& back() const;

        void clear();
        sz   size() const;
        sz   capacity() const;
        bool empty() const;
        void reserve(sz capacity);

        T*       begin();
        T*       end();
        const T* begin() const;
        const T* end() const;

        void destroy();

    private:
        T* m_data     = nullptr;
        sz m_size     = 0;
        sz m_capacity = 0;
        sz m_head     = 0;    // index of front element
        sz m_tail     = 0;    // index where next element will be inserted

        T*       ptr_at(size_t index) noexcept { return reinterpret_cast<T*>(reinterpret_cast<char*>(m_data) + index * sizeof(T)); }
        const T* ptr_at(size_t index) const noexcept { return reinterpret_cast<const T*>(reinterpret_cast<const char*>(m_data) + index * sizeof(T)); }
    };

    // ---------------- Implementation ----------------

    template<typename T>
    RZQueue<T>::RZQueue(sz initialCapacity)
    {
        reserve(initialCapacity);
    }

    template<typename T>
    RZQueue<T>::~RZQueue()
    {
        clear();
        destroy();
    }

    template<typename T>
    RZQueue<T>::RZQueue(RZQueue&& other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity), m_head(other.m_head), m_tail(other.m_tail)
    {
        other.m_data     = nullptr;
        other.m_size     = 0;
        other.m_capacity = 0;
        other.m_head     = 0;
        other.m_tail     = 0;
    }

    template<typename T>
    RZQueue<T>& RZQueue<T>::operator=(RZQueue&& other) noexcept
    {
        if (this != &other) {
            destroy();

            m_data     = other.m_data;
            m_size     = other.m_size;
            m_capacity = other.m_capacity;
            m_head     = other.m_head;
            m_tail     = other.m_tail;

            other.m_data     = nullptr;
            other.m_size     = 0;
            other.m_capacity = 0;
            other.m_head     = 0;
            other.m_tail     = 0;
        }
        return *this;
    }

    template<typename T>
    void RZQueue<T>::reserve(sz capacity)
    {
        if (capacity > m_capacity || !m_data) {
            m_data = static_cast<T*>(rz_malloc(capacity * sizeof(T), 16));
            if (!m_data)
                RAZIX_CORE_ERROR("[RZQueue] Failed to allocate memory for RZQueue");

            m_capacity = capacity;
        }
    }

    template<typename T>
    void RZQueue<T>::destroy()
    {
        if (m_data) {
            rz_free(m_data);
            m_data     = nullptr;
            m_size     = 0;
            m_capacity = 0;
            m_head     = 0;
            m_tail     = 0;
        }
    }

    template<typename T>
    bool RZQueue<T>::empty() const
    {
        return m_size == 0;
    }

    template<typename T>
    sz RZQueue<T>::size() const
    {
        return m_size;
    }

    template<typename T>
    sz RZQueue<T>::capacity() const
    {
        return m_capacity;
    }

    template<typename T>
    void RZQueue<T>::clear()
    {
        m_tail = 0;
        m_head = 0;
        m_size = 0;
    }

    template<typename T>
    T& RZQueue<T>::front()
    {
        T* p = ptr_at(m_head % m_capacity);
        return *p;
    }

    template<typename T>
    const T& RZQueue<T>::front() const
    {
        const T* p = ptr_at(m_head % m_capacity);
        return *p;
    }

    template<typename T>
    T& RZQueue<T>::back()
    {
        T* p = ptr_at((m_tail - 1) % m_capacity);
        return *p;
    }

    template<typename T>
    const T& RZQueue<T>::back() const
    {
        const T* p = ptr_at((m_tail - 1) % m_capacity);
        return *p;
    }

    template<typename T>
    T RZQueue<T>::pop()
    {
        T* p   = ptr_at(m_head % m_capacity);
        T  val = rz_move(*p);
        m_head = (m_head + 1) % m_capacity;
        m_size--;
        return val;
    }

    template<typename T>
    template<typename... Args>
    void RZQueue<T>::emplace(Args&&... args)
    {
        T* p = ptr_at(m_tail % m_capacity);
        new (p) T(std::forward<Args>(args)...);
        m_tail = (m_tail + 1) % m_capacity;
        m_size++;
    }

    template<typename T>
    void RZQueue<T>::push(const T& val)
    {
        T* p = ptr_at(m_tail % m_capacity);
        new (p) T(val);
        m_tail = (m_tail + 1) % m_capacity;
        m_size++;
    }

    template<typename T>
    void RZQueue<T>::push(T&& val)
    {
        T* p = ptr_at(m_tail % m_capacity);
        new (p) T(rz_move(val));
        m_tail = (m_tail + 1) % m_capacity;
        m_size++;
    }

    template<typename T>
    T* RZQueue<T>::begin()
    {
        return ptr_at(m_head);
    }

    template<typename T>
    T* RZQueue<T>::end()
    {
        return ptr_at(m_tail);
    }

    template<typename T>
    const T* RZQueue<T>::begin() const
    {
        return ptr_at(m_head);
    }

    template<typename T>
    const T* RZQueue<T>::end() const
    {
        return ptr_at(m_tail);
    }

}    // namespace Razix

#endif    // _RZ_QUEUE_H_
