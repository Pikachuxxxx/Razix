#ifndef _RZ_INIT_LIST_H_
#define _RZ_INIT_LIST_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/utility.h"

namespace Razix {

    template<typename T>
    class rz_initializer_list
    {
    public:
        using value_type      = T;
        using const_reference = const T&;
        using const_iterator  = const T*;
        using size_type       = size_t;

    private:
        const T* m_data;
        size_t   m_size;

        // only the compiler can construct this
        constexpr rz_initializer_list(const T* data, size_t size) noexcept
            : m_data(data), m_size(size)
        {
        }

    public:
        constexpr rz_initializer_list() noexcept
            : m_data(nullptr), m_size(0)
        {
        }

        constexpr size_type size() const noexcept
        {
            return m_size;
        }

        constexpr const_iterator begin() const noexcept
        {
            return m_data;
        }

        constexpr const_iterator end() const noexcept
        {
            return m_data + m_size;
        }

        constexpr const_reference operator[](size_type index) const noexcept
        {
            return m_data[index];
        }
    };
}    // namespace Razix
#endif
