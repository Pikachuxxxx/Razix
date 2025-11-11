#ifndef _RZ_ARRAYS_H_
#define _RZ_ARRAYS_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/std/utility.h"

#define RZ_DEFAULT_ARRAY_CAPACITY 64

namespace Razix {

    template<typename T, size_t N>
    class RZFixedArray
    {
    public:
        using value_type      = T;
        using size_type       = size_t;
        using reference       = T&;
        using const_reference = const T&;
        using pointer         = T*;
        using const_pointer   = const T*;
        using iterator        = T*;
        using const_iterator  = const T*;

        RZFixedArray() = default;
        ~RZFixedArray();

        RZFixedArray(const RZFixedArray& other);
        RZFixedArray& operator=(const RZFixedArray& other);

        RZFixedArray(RZFixedArray&& other) noexcept;
        RZFixedArray& operator=(RZFixedArray&& other) noexcept;

        reference       operator[](size_type index);
        const_reference operator[](size_type index) const;

        reference       at(size_type index);
        const_reference at(size_type index) const;

        reference       front();
        const_reference front() const;
        reference       back();
        const_reference back() const;

        iterator       begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator       end();
        const_iterator end() const;
        const_iterator cend() const;

        bool                empty() const;
        size_type           size() const;
        constexpr size_type max_size() const;
        constexpr size_type capacity() const;
        void                clear();

        void push_back(const T& value);
        void push_back(T&& value);
        template<typename... Args>
        reference emplace_back(Args&&... args);
        void      pop_back();
        void      resize(size_type count, const T& value = T{});

    private:
        template<typename... Args>
        void construct(size_type index, Args&&... args)
        {
            // Note: Why new? Because placement new constructs the object in the pre-allocated memory 
            // It also calls the constructor of T with the provided arguments, which is essential for proper initialization.
            new (&reinterpret_cast<T*>(m_Data)[index]) T(std::forward<Args>(args)...);
        }

        void destroy(size_type index)
        {
            reinterpret_cast<T*>(m_Data)[index].~T();
        }

    private:
        alignas(T) unsigned char m_Data[sizeof(T) * N];
        size_type m_Size = 0;
    };

    //-------------------------------------
    // Implementation RZFixedArray
    //-------------------------------------

    template<typename T, size_t N>
    RZFixedArray<T, N>::~RZFixedArray()
    {
        clear();
    }

    template<typename T, size_t N>
    RZFixedArray<T, N>::RZFixedArray(const RZFixedArray& other)
    {
        for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
            construct(m_Size, other[m_Size]);
    }

    template<typename T, size_t N>
    RZFixedArray<T, N>& RZFixedArray<T, N>::operator=(const RZFixedArray& other)
    {
        if (this != &other) {
            clear();
            for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
                construct(m_Size, other[m_Size]);
        }
        return *this;
    }

    template<typename T, size_t N>
    RZFixedArray<T, N>::RZFixedArray(RZFixedArray&& other) noexcept
    {
        for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
            construct(m_Size, rz_move(other[m_Size]));
        other.clear();
    }

    template<typename T, size_t N>
    RZFixedArray<T, N>& RZFixedArray<T, N>::operator=(RZFixedArray&& other) noexcept
    {
        if (this != &other) {
            clear();
            for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
                // TODO: Replace std::forward with rz_forward when implemented
                construct(m_Size, rz_move(other[m_Size]));
            other.clear();
        }
        return *this;
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::reference
    RZFixedArray<T, N>::operator[](size_type index)
    {
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_reference
    RZFixedArray<T, N>::operator[](size_type index) const
    {
        return reinterpret_cast<const T*>(m_Data)[index];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::reference
    RZFixedArray<T, N>::at(size_type index)
    {
        RAZIX_CORE_ASSERT(index < m_Size, "RZFixedArray: Index out of bounds!");
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_reference
    RZFixedArray<T, N>::at(size_type index) const
    {
        RAZIX_CORE_ASSERT(index < m_Size, "RZFixedArray: Index out of bounds!");
        return reinterpret_cast<const T*>(m_Data)[index];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::reference
    RZFixedArray<T, N>::front()
    {
        return reinterpret_cast<T*>(m_Data)[0];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_reference
    RZFixedArray<T, N>::front() const
    {
        return reinterpret_cast<const T*>(m_Data)[0];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::reference
    RZFixedArray<T, N>::back()
    {
        return reinterpret_cast<T*>(m_Data)[m_Size - 1];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_reference
    RZFixedArray<T, N>::back() const
    {
        return reinterpret_cast<const T*>(m_Data)[m_Size - 1];
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::iterator
    RZFixedArray<T, N>::begin()
    {
        return reinterpret_cast<T*>(m_Data);
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_iterator
    RZFixedArray<T, N>::begin() const
    {
        return reinterpret_cast<const T*>(m_Data);
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_iterator
    RZFixedArray<T, N>::cbegin() const
    {
        return reinterpret_cast<const T*>(m_Data);
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::iterator
    RZFixedArray<T, N>::end()
    {
        return reinterpret_cast<T*>(m_Data) + m_Size;
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_iterator
    RZFixedArray<T, N>::end() const
    {
        return reinterpret_cast<const T*>(m_Data) + m_Size;
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::const_iterator
    RZFixedArray<T, N>::cend() const
    {
        return reinterpret_cast<const T*>(m_Data) + m_Size;
    }

    template<typename T, size_t N>
    bool RZFixedArray<T, N>::empty() const
    {
        return m_Size == 0;
    }

    template<typename T, size_t N>
    typename RZFixedArray<T, N>::size_type
    RZFixedArray<T, N>::size() const
    {
        return m_Size;
    }

    template<typename T, size_t N>
    constexpr typename RZFixedArray<T, N>::size_type
    RZFixedArray<T, N>::max_size() const
    {
        return N;
    }

    template<typename T, size_t N>
    constexpr typename RZFixedArray<T, N>::size_type
    RZFixedArray<T, N>::capacity() const
    {
        return N;
    }

    template<typename T, size_t N>
    void RZFixedArray<T, N>::push_back(const T& value)
    {
        RAZIX_CORE_ASSERT(m_Size < N, "RZFixedArray: Exceeded fixed capacity of {0}", N);
        construct(m_Size++, value);
    }

    template<typename T, size_t N>
    void RZFixedArray<T, N>::push_back(T&& value)
    {
        RAZIX_CORE_ASSERT(m_Size < N, "RZFixedArray: Exceeded fixed capacity of {0}", N);
        construct(m_Size++, rz_move(value));
    }

    template<typename T, size_t N>
    template<typename... Args>
    typename RZFixedArray<T, N>::reference
    RZFixedArray<T, N>::emplace_back(Args&&... args)
    {
        RAZIX_CORE_ASSERT(m_Size < N, "RZFixedArray: Exceeded fixed capacity of {0}", N);
        construct(m_Size++, std::forward<Args>(args)...);
        return back();
    }

    template<typename T, size_t N>
    void RZFixedArray<T, N>::pop_back()
    {
        RAZIX_CORE_ASSERT(m_Size > 0, "RZFixedArray: Cannot pop_back from empty array");
        destroy(--m_Size);
    }

    template<typename T, size_t N>
    void RZFixedArray<T, N>::clear()
    {
        while (m_Size > 0)
            destroy(--m_Size);
    }

    template<typename T, size_t N>
    void RZFixedArray<T, N>::resize(size_type count, const T& value)
    {
        RAZIX_CORE_ASSERT(count <= N, "RZFixedArray: Cannot resize beyond fixed capacity of {0}", N);
        while (m_Size < count)
            construct(m_Size++, value);
        while (m_Size > count)
            destroy(--m_Size);
    }
}    // namespace Razix

#endif    // _RZ_ARRAYS_H_
