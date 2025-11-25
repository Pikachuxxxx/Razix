#ifndef _RZ_ARRAYS_H_
#define _RZ_ARRAYS_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Core/std/utility.h"

#define RZ_DEFAULT_ARRAY_CAPACITY 64
#define RZ_ARRAY_GROWTH_FACTOR    2

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

        RZFixedArray(std::initializer_list<T> init);

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
        for (u32 i = 0; i < m_Size; ++i)
            destroy(m_Size);
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
                construct(m_Size, rz_move(other[m_Size]));
            other.clear();
        }
        return *this;
    }

    template<typename T, size_t N>
    RZFixedArray<T, N>::RZFixedArray(std::initializer_list<T> init)
    {
        reserve(init.size());
        for (const auto& item: init) {
            push_back(item);
        }
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
        // TODO: Replace std::forward with rz_forward when implemented
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

    //----------------------------------------------------------
    // RZDynamicArray
    //----------------------------------------------------------

    template<typename T>
    class RZDynamicArray
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

        RZDynamicArray() = default;
        RZDynamicArray(size_type initialCapacity);
        RZDynamicArray(size_type initialCapacity, const T& value);
        ~RZDynamicArray();

        RZDynamicArray(const RZDynamicArray& other);
        RZDynamicArray& operator=(const RZDynamicArray& other);

        RZDynamicArray(RZDynamicArray&& other) noexcept;
        RZDynamicArray& operator=(RZDynamicArray&& other) noexcept;

        RZDynamicArray(std::initializer_list<T> init);

        reference       operator[](size_type index);
        const_reference operator[](size_type index) const;
        reference       at(size_type index);
        const_reference at(size_type index) const;
        reference       front();
        const_reference front() const;
        reference       back();
        const_reference back() const;
        iterator        begin();
        const_iterator  begin() const;
        const_iterator  cbegin() const;
        iterator        end();
        const_iterator  end() const;
        const_iterator  cend() const;
        bool            empty() const;
        size_type       size() const;
        size_type       capacity() const;
        void            clear();
        void            erase(size_type index);
        T*              data();
        const T*        data() const;

        void push_back(const T& value);
        void push_back(T&& value);
        template<typename... Args>
        reference emplace_back(Args&&... args);
        void      pop_back();
        void      resize(size_type count, const T& value = T{});
        void      reserve(size_type newCapacity);

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
        T*        m_Data     = NULL;
        size_type m_Size     = 0;
        size_type m_Capacity = 0;
    };

    //----------------------------------------------------------
    // RZDynamicArray Implementation
    //----------------------------------------------------------

    template<typename T>
    RZDynamicArray<T>::RZDynamicArray(size_type initialCapacity)
    {
        if (initialCapacity == 0)
            return;
        m_Data     = reinterpret_cast<T*>(rz_malloc_aligned(sizeof(T) * initialCapacity));
        m_Capacity = initialCapacity;
        memset(m_Data, 0x0, sizeof(T) * initialCapacity);
    };

    template<typename T>
    RZDynamicArray<T>::RZDynamicArray(size_type initialCapacity, const T& value)
    {
        if (initialCapacity == 0)
            return;
        m_Data     = reinterpret_cast<T*>(rz_malloc_aligned(sizeof(T) * initialCapacity));
        m_Size     = 0;
        m_Capacity = initialCapacity;
        for (m_Size = 0; m_Size < initialCapacity; ++m_Size)
            construct(m_Size, value);
    };

    template<typename T>
    RZDynamicArray<T>::~RZDynamicArray()
    {
        clear();
        if (m_Data)
            rz_free(m_Data);
    }

    template<typename T>
    RZDynamicArray<T>::RZDynamicArray(const RZDynamicArray& other)
    {
        if (m_Data)
            rz_free(m_Data);
        m_Data     = reinterpret_cast<T*>(rz_malloc_aligned(sizeof(T) * other.m_Capacity));
        m_Capacity = other.m_Capacity;
        m_Size     = 0;
        for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
            construct(m_Size, other[m_Size]);
    }

    template<typename T>
    RZDynamicArray<T>& RZDynamicArray<T>::operator=(const RZDynamicArray& other)
    {
        if (this != &other) {
            clear();
            // Allocate new memory if the current capacity is less than the other array's capacity
            // Don't care about earlier data, we are overwriting it
            if (m_Capacity < other.m_Capacity) {
                if (m_Data)
                    rz_free(m_Data);
                m_Data     = reinterpret_cast<T*>(rz_malloc_aligned(sizeof(T) * other.m_Capacity));
                m_Capacity = other.m_Capacity;
            }
            m_Size = 0;
            for (m_Size = 0; m_Size < other.m_Size; ++m_Size)
                construct(m_Size, other[m_Size]);
        }
        return *this;
    }

    template<typename T>
    RZDynamicArray<T>::RZDynamicArray(RZDynamicArray&& other) noexcept
    {
        m_Data           = other.m_Data;
        m_Size           = other.m_Size;
        m_Capacity       = other.m_Capacity;
        other.m_Data     = NULL;
        other.m_Size     = 0;
        other.m_Capacity = 0;
    }

    template<typename T>
    RZDynamicArray<T>& RZDynamicArray<T>::operator=(RZDynamicArray&& other) noexcept
    {
        if (this != &other) {
            clear();
            if (m_Data)
                rz_free(m_Data);
            m_Data           = other.m_Data;
            m_Size           = other.m_Size;
            m_Capacity       = other.m_Capacity;
            other.m_Data     = NULL;
            other.m_Size     = 0;
            other.m_Capacity = 0;
        }
        return *this;
    }

    template<typename T>
    RZDynamicArray<T>::RZDynamicArray(std::initializer_list<T> init)
    {
        reserve(init.size());
        for (const auto& item: init) {
            push_back(item);
        }
    }

    template<typename T>
    typename RZDynamicArray<T>::reference RZDynamicArray<T>::operator[](size_type index)
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(index < m_Size, "RZDynamicArray: Index out of bounds!");
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T>
    typename RZDynamicArray<T>::const_reference RZDynamicArray<T>::operator[](size_type index) const
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(index < m_Size, "RZDynamicArray: Index out of bounds!");
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T>
    typename RZDynamicArray<T>::reference RZDynamicArray<T>::at(size_type index)
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(index < m_Size, "RZDynamicArray: Index out of bounds!");
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T>
    typename RZDynamicArray<T>::const_reference RZDynamicArray<T>::at(size_type index) const
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(index < m_Size, "RZDynamicArray: Index out of bounds!");
        return reinterpret_cast<T*>(m_Data)[index];
    }

    template<typename T>
    typename RZDynamicArray<T>::reference RZDynamicArray<T>::front()
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return reinterpret_cast<T*>(m_Data)[0];
    }

    template<typename T>
    typename RZDynamicArray<T>::const_reference RZDynamicArray<T>::front() const
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return reinterpret_cast<T*>(m_Data)[0];
    }

    template<typename T>
    typename RZDynamicArray<T>::reference RZDynamicArray<T>::back()
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return reinterpret_cast<T*>(m_Data)[m_Size - 1];
    }

    template<typename T>
    typename RZDynamicArray<T>::const_reference RZDynamicArray<T>::back() const
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return reinterpret_cast<T*>(m_Data)[m_Size - 1];
    }

    template<typename T>
    typename RZDynamicArray<T>::iterator RZDynamicArray<T>::begin()
    {
        return reinterpret_cast<T*>(m_Data);
    }

    template<typename T>
    typename RZDynamicArray<T>::const_iterator RZDynamicArray<T>::begin() const
    {
        return reinterpret_cast<T*>(m_Data);
    }

    template<typename T>
    typename RZDynamicArray<T>::const_iterator RZDynamicArray<T>::cbegin() const
    {
        return reinterpret_cast<T*>(m_Data);
    }

    template<typename T>
    typename RZDynamicArray<T>::iterator RZDynamicArray<T>::end()
    {
        return reinterpret_cast<T*>(m_Data) + m_Size;
    }

    template<typename T>
    typename RZDynamicArray<T>::const_iterator RZDynamicArray<T>::end() const
    {
        return reinterpret_cast<T*>(m_Data) + m_Size;
    }

    template<typename T>
    typename RZDynamicArray<T>::const_iterator RZDynamicArray<T>::cend() const
    {
        return reinterpret_cast<T*>(m_Data) + m_Size;
    }

    template<typename T>
    bool RZDynamicArray<T>::empty() const
    {
        return m_Size == 0;
    }

    template<typename T>
    typename RZDynamicArray<T>::size_type RZDynamicArray<T>::size() const
    {
        return m_Size;
    }

    template<typename T>
    typename RZDynamicArray<T>::size_type RZDynamicArray<T>::capacity() const
    {
        return m_Capacity;
    }

    template<typename T>
    void RZDynamicArray<T>::clear()
    {
        for (RZDynamicArray<T>::size_type i = 0; i < m_Size; ++i)
            destroy(i);
        m_Size = 0;
    }

    template<typename T>
    void RZDynamicArray<T>::erase(size_type index)
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(index < m_Size, "RZDynamicArray: Index out of bounds!");
        // Destroy the element at the specified index
        destroy(index);
        // Shift elements to fill the gap
        for (size_type i = index; i < m_Size - 1; ++i) {
            construct(i, rz_move((*this)[i + 1]));
            destroy(i + 1);
        }
        --m_Size;
    }

    template<typename T>
    T* RZDynamicArray<T>::data()
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return m_Data;
    }

    template<typename T>
    const T* RZDynamicArray<T>::data() const
    {
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot access uninitialized array. Call reserve() first to allocate memory.");
        return m_Data;
    }

    template<typename T>
    void RZDynamicArray<T>::push_back(const T& value)
    {
        reserve(1 + m_Size);
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot push_back to uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(m_Size < m_Capacity, "RZDynamicArray::push_back size exceeded at capacity: {}", m_Capacity);
        construct(m_Size++, value);
    }

    template<typename T>
    void RZDynamicArray<T>::push_back(T&& value)
    {
        reserve(1 + m_Size);
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot push_back to uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(m_Size < m_Capacity, "RZDynamicArray::push_back size exceeded at capacity: {}", m_Capacity);
        construct(m_Size++, std::forward<T>(value));
    }

    template<typename T>
    template<typename... Args>
    typename RZDynamicArray<T>::reference RZDynamicArray<T>::emplace_back(Args&&... args)
    {
        reserve(1 + m_Size);
        RAZIX_CORE_ASSERT(m_Data != NULL, "RZDynamicArray: Cannot push_back to uninitialized array. Call reserve() first to allocate memory.");
        RAZIX_CORE_ASSERT(m_Size < m_Capacity, "RZDynamicArray::push_back size exceeded at capacity: {}", m_Capacity);
        construct(m_Size++, std::forward<Args>(args)...);
        return m_Data[m_Size - 1];
    }

    template<typename T>
    void RZDynamicArray<T>::pop_back()
    {
        RAZIX_CORE_ASSERT(!empty(), "RZDynamicArray: Cannot pop_back() from empty array");
        destroy(--m_Size);
    }

    template<typename T>
    void RZDynamicArray<T>::resize(size_type count, const T& value)
    {
        if (count > m_Capacity)
            reserve(count);

        while (m_Size < count)
            construct(m_Size++, value);
        while (m_Size > count)
            destroy(--m_Size);
    }

    template<typename T>
    void RZDynamicArray<T>::reserve(size_type newCapacity)
    {
        RAZIX_CORE_ASSERT(newCapacity > 0, "RZDynamicArray: Cannot reserve with capacity of 0");

        if (newCapacity <= m_Capacity)
            return;

        while (m_Capacity < newCapacity)
            m_Capacity = (m_Capacity == 0) ? RZ_DEFAULT_ARRAY_CAPACITY : m_Capacity * RZ_ARRAY_GROWTH_FACTOR;

        T* newData = reinterpret_cast<T*>(rz_malloc_aligned(m_Capacity * sizeof(T)));
        RAZIX_CORE_ASSERT(newData != NULL, "RZDynamicArray: Memory allocation failed in reserve()");

        // Move the old data to the new newData
        if (m_Data != NULL && m_Size > 0) {
            // Do element-wise move construction to the new Memory
            // We cannot just do a memcpy because that would skip the constructor call and lead to undefined behavior for complex types
            for (size_type i = 0; i < m_Size; ++i) {
                new (&newData[i]) T(rz_move(m_Data[i]));
                m_Data[i].~T();
            }
            rz_free(m_Data);
        }

        m_Data = newData;
    }

}    // namespace Razix

#endif    // _RZ_ARRAYS_H_
