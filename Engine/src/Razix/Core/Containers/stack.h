#ifndef _RZ_STACK_H_
#define _RZ_STACK_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

// Tasks:
// - [x] create a simple stack templated class (push/pop/size etc.)
// - [ ] write gtests

#define RZ_DEFAULT_STACK_CAPACITY 64

namespace Razix {

    template<typename T>
    class RZStack final
    {
    public:
        RZStack(sz initialCapacity = RZ_DEFAULT_STACK_CAPACITY);
        ~RZStack();

        RAZIX_NONCOPYABLE_CLASS(RZStack);

        RZStack(RZStack&& other) noexcept;
        RZStack& operator=(RZStack&& other) noexcept;

        template<typename... Args>
        void     emplace(Args&&... args);
        void     push(const T& val);
        void     push(T&& val);
        T        pop();
        T&       top();
        const T& top() const;
        void     clear();
        sz       size() const;
        sz       capacity() const;
        bool     empty() const;
        void     reserve(sz capacity);
        T*       begin();
        T*       end();
        const T* begin() const;
        const T* end() const;
        void     destroy();

    private:
        T* m_data     = NULL;
        sz m_size     = 0;
        sz m_capacity = 0;

        T*       ptr_at(size_t index) noexcept { return reinterpret_cast<T*>(reinterpret_cast<char*>(m_data) + index * sizeof(T)); }
        const T* ptr_at(size_t index) const noexcept { return reinterpret_cast<const T*>(reinterpret_cast<const char*>(m_data) + index * sizeof(T)); }
    };

    // ---------------- Implementation ----------------

    template<typename T>
    RZStack<T>::RZStack(sz initialCapacity /*= 64*/)
        : m_data(NULL), m_size(0), m_capacity(initialCapacity)
    {
        if (!initialCapacity) m_capacity = RZ_DEFAULT_STACK_CAPACITY;
        reserve(m_capacity);
    }

    template<typename T>
    RZStack<T>::~RZStack()
    {
        clear();
        destroy();
    }

    template<typename T>
    RZStack<T>::RZStack(RZStack&& other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
    {
        other.m_data     = NULL;
        other.m_size     = 0;
        other.m_capacity = 0;
    }

    template<typename T>
    RZStack<T>& RZStack<T>::operator=(RZStack&& other) noexcept
    {
        if (this != &other) {
            destroy();

            m_data     = other.m_data;
            m_size     = other.m_size;
            m_capacity = other.m_capacity;

            other.m_data     = NULL;
            other.m_size     = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    template<typename T>
    template<typename... Args>
    void RZStack<T>::emplace(Args&&... args)
    {
        T* p = ptr_at(m_size);
        new (p) T(std::forward<Args>(args)...);
        ++m_size;
    }

    template<typename T>
    void RZStack<T>::push(const T& val)
    {
        T* p = ptr_at(m_size);
        new (p) T(val);
        ++m_size;
    }

    template<typename T>
    void RZStack<T>::push(T&& val)
    {
        T* p = ptr_at(m_size);
        new (p) T(std::move(val));
        ++m_size;
    }

    template<typename T>
    T RZStack<T>::pop()
    {
        RAZIX_CORE_ASSERT(m_size > 0, "RZStack::pop() called on empty stack");

        T* p   = ptr_at(m_size - 1);
        T  val = std::move(*p);
        p->~T();
        --m_size;
        return val;
    }

    template<typename T>
    T& RZStack<T>::top()
    {
        RAZIX_CORE_ASSERT(m_size > 0, "RZStack::top() called on empty stack");

        T* p = ptr_at(m_size - 1);
        return *p;
    }

    template<typename T>
    const T& RZStack<T>::top() const
    {
        RAZIX_CORE_ASSERT(m_size > 0, "RZStack::top() called on empty stack");

        const T* p = ptr_at(m_size - 1);
        return *p;
    }

    template<typename T>
    void RZStack<T>::clear()
    {
        for (sz i = 0; i < m_size; ++i) {
            ptr_at(i)->~T();
        }
        m_size = 0;
    }

    template<typename T>
    sz RZStack<T>::size() const
    {
        return m_size;
    }

    template<typename T>
    sz RZStack<T>::capacity() const
    {
        return m_capacity;
    }

    template<typename T>
    bool RZStack<T>::empty() const
    {
        return m_size == 0;
    }

    template<typename T>
    void RZStack<T>::reserve(sz capacity)
    {
        if (capacity > m_capacity || !m_data) {
            m_data = (T*) Razix::Memory::RZMalloc(capacity * sizeof(T), 16);
            if (!m_data) {
                RAZIX_CORE_ERROR("[RZStack] failed to allocated memory for RZStack");
            }
            m_capacity = capacity;
        }
    }

    template<typename T>
    T* RZStack<T>::begin()
    {
        return ptr_at(0);
    }

    template<typename T>
    T* RZStack<T>::end()
    {
        return ptr_at(m_size);
    }

    template<typename T>
    const T* RZStack<T>::begin() const
    {
        return ptr_at(0);
    }

    template<typename T>
    const T* RZStack<T>::end() const
    {
        return ptr_at(m_size);
    }

    template<typename T>
    void RZStack<T>::destroy()
    {
        if (m_data) {
            Razix::Memory::RZFree(m_data);
            m_size     = 0;
            m_capacity = 0;
            m_data     = NULL;
        }
    }

}    // namespace Razix

#endif    // _RZ_STACK_H_