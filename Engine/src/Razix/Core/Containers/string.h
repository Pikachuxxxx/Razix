#ifndef _RZ_STRING_H_
#define _RZ_STRING_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/type_traits.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Core/Containers/hash_functors.h"

#include <typeinfo>    // for std::type_info used in rz_to_string

#define RAZIX_SSO_STRING_SIZE 64

namespace Razix {

    extern sz    rz_strlen(const char* str);
    extern i32   rz_strcmp(const char* s1, const char* s2);
    extern i32   rz_memcmp(const void* ptr1, const void* ptr2, sz count);
    extern void* rz_memcpy(void* dst, const void* src, sz count);

    // [header declarations taken from]: https://en.cppreference.com/w/cpp/string/basic_string.html
    class RAZIX_MEM_ALIGN_16 RAZIX_API RZString
    {
    public:
        using value_type      = char;
        using difference_type = ptrdiff_t;
        using reference       = char&;
        using const_reference = const char&;
        using pointer         = char*;
        using const_pointer   = const char*;

        // no string
        static constexpr sz npos = static_cast<sz>(-1);

        RZString()
            : m_length(0), m_capacity(RAZIX_SSO_STRING_SIZE), m_is_using_heap(false)
        {
            m_data.sso[0] = '\0';
        }
        ~RZString()
        {
            if (m_is_using_heap && m_data.ptr != NULL)
                rz_free(m_data.ptr);
        }

        RZString(const RZString& other)
        {
            if (m_is_using_heap) {
                rz_free(m_data.ptr);
                m_data.ptr = NULL;
            }

            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr = (char*) rz_malloc(m_capacity, RAZIX_CACHE_LINE_ALIGN);
                rz_memcpy(m_data.ptr, other.m_data.ptr, m_length);
                m_data.ptr[m_length] = '\0';
            } else {
                // copy string as-is if it's SSO
                rz_memcpy(m_data.sso, other.m_data.sso, RAZIX_SSO_STRING_SIZE);
                m_data.sso[m_length] = '\0';
            }
        }

        RZString& operator=(const RZString& other)
        {
            if (m_is_using_heap) {
                rz_free(m_data.ptr);
                m_data.ptr = NULL;
            }

            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr = (char*) rz_malloc(m_capacity, RAZIX_CACHE_LINE_ALIGN);
                rz_memcpy(m_data.ptr, other.m_data.ptr, m_length);
                m_data.ptr[m_length] = '\0';
            } else {
                // copy string as-is if it's SSO
                rz_memcpy(m_data.sso, other.m_data.sso, RAZIX_SSO_STRING_SIZE);
                m_data.sso[m_length] = '\0';
            }

            return *this;
        }

        RZString(RZString&& other) noexcept
        {
            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr           = other.m_data.ptr;
                m_data.ptr[m_length] = '\0';
            } else {
                rz_memcpy(m_data.sso, other.m_data.sso, m_length);
                m_data.sso[m_length] = '\0';
            }

            other.m_is_using_heap = false;
            other.m_length        = 0;
            other.m_capacity      = RAZIX_SSO_STRING_SIZE;
            other.m_data.ptr      = NULL;
        }

        RZString& operator=(RZString&& other) noexcept
        {
            if (m_data.ptr && m_is_using_heap)
                rz_free(m_data.ptr);

            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr           = other.m_data.ptr;
                m_data.ptr[m_length] = '\0';
            } else {
                rz_memcpy(m_data.sso, other.m_data.sso, m_length);
                m_data.sso[m_length] = '\0';
            }

            other.m_is_using_heap = false;
            other.m_length        = 0;
            other.m_capacity      = RAZIX_SSO_STRING_SIZE;
            other.m_data.ptr      = NULL;

            return *this;
        }

        RZString(const char* str);
        RZString(const char* str, sz count);
        RZString(const RZString& other, sz pos, sz count = npos);
        RZString(sz count, char ch);

        reference       at(sz pos);
        const_reference at(sz pos) const;
        reference       operator[](sz pos);
        const_reference operator[](sz pos) const;
        reference       front();
        const_reference front() const;
        reference       back();
        const_reference back() const;
        pointer         data();
        const_pointer   data() const;
        const char*     c_str() const;

        pointer       begin();
        const_pointer begin() const;
        pointer       end();
        const_pointer end() const;

        bool empty() const;
        sz   size() const;
        sz   length() const;
        sz   capacity() const;
        sz   max_size() const;
        void reserve(sz new_capacity);
        void resize(sz new_length);
        void clear();
        void setLength(sz length);
        sz   hash() const;

        RZString& append(const RZString& str);
        RZString& append(const RZString& str, sz pos, sz count = npos);
        RZString& append(const char* str);
        RZString& append(const char* str, sz count);
        RZString& append(sz count, char ch);

        RZString& operator+=(const RZString& str) { return append(str); }
        RZString& operator+=(const char* str) { return append(str); }
        RZString& operator+=(char ch) { return append(1, ch); }

        RZString& insert(sz pos, const RZString& str);
        RZString& insert(sz pos, const char* str);
        RZString& insert(sz pos, sz count, char ch);
        RZString& insert(sz pos, const char* str, sz count);

        RZString& erase(sz pos = 0, sz count = npos);

        RZString& replace(sz pos, sz count, const RZString& str);
        RZString& replace(sz pos, sz count, const char* str);
        RZString& replace(sz pos, sz count, char ch);

        RZString& push_back(char ch);
        RZString& pop_back();

        void swap(RZString& other);

        sz find(const RZString& str, sz pos = 0) const;
        sz find(const char* str, sz pos = 0) const;
        sz find(char ch, sz pos = 0) const;

        sz rfind(const RZString& str, sz pos = npos) const;
        sz rfind(const char* str, sz pos = npos) const;
        sz rfind(char ch, sz pos = npos) const;

        sz find_first_of(const RZString& str, sz pos = 0) const;
        sz find_first_of(const char* str, sz pos = 0) const;
        sz find_first_of(char ch, sz pos = 0) const;

        sz find_last_of(const RZString& str, sz pos = npos) const;
        sz find_last_of(const char* str, sz pos = npos) const;
        sz find_last_of(char ch, sz pos = npos) const;

        sz find_first_not_of(const RZString& str, sz pos = 0) const;
        sz find_first_not_of(const char* str, sz pos = 0) const;
        sz find_first_not_of(char ch, sz pos = 0) const;

        sz find_last_not_of(const RZString& str, sz pos = npos) const;
        sz find_last_not_of(const char* str, sz pos = npos) const;
        sz find_last_not_of(char ch, sz pos = npos) const;

        RZString substr(sz pos = 0, sz count = npos) const;

        i32 compare(const RZString& str) const;
        i32 compare(sz pos1, sz count1, const RZString& str) const;
        i32 compare(sz pos1, sz count1, const RZString& str, sz pos2, sz count2) const;
        i32 compare(const char* str) const;
        i32 compare(sz pos1, sz count1, const char* str) const;
        i32 compare(sz pos1, sz count1, const char* str, sz count2) const;

        RZString& operator=(const char* str);
        RZString& operator=(char ch);

        bool operator==(const RZString& other) const;
        bool operator==(const char* str) const;
        bool operator!=(const RZString& other) const;
        bool operator!=(const char* str) const;
        bool operator<(const RZString& other) const;
        bool operator<=(const RZString& other) const;
        bool operator>(const RZString& other) const;
        bool operator>=(const RZString& other) const;

        explicit operator const char*() const { return c_str(); }

    private:
        union
        {
            char  sso[RAZIX_SSO_STRING_SIZE];
            char* ptr;
        } m_data = {};

        sz   m_length        = 0;
        sz   m_capacity      = RAZIX_SSO_STRING_SIZE;
        bool m_is_using_heap = false;
        bool _pad0[7]        = {};
        u32  _pad1[2]        = {};

        RAZIX_API friend RZString operator+(const RZString& lhs, const RZString& rhs);
        RAZIX_API friend RZString operator+(const RZString& lhs, const char* rhs);
        RAZIX_API friend RZString operator+(const char* lhs, const RZString& rhs);
        RAZIX_API friend RZString operator+(const RZString& lhs, char rhs);
        RAZIX_API friend RZString operator+(char lhs, const RZString& rhs);

        // NOTE: RZString intentionally does not provide operator<< or operator>>
        // to avoid STL header dependencies. Use .c_str() instead:
        //   std::cout << myString.c_str();
        //   EXPECT_EQ(myString.c_str(), "expected");
    };

    //--------------------------------------------------------------------
    // Friend operators - for both seperate operations
    //--------------------------------------------------------------------

    RAZIX_API RZString operator+(const RZString& lhs, const RZString& rhs);
    RAZIX_API RZString operator+(const RZString& lhs, const char* rhs);
    RAZIX_API RZString operator+(const char* lhs, const RZString& rhs);
    RAZIX_API RZString operator+(const RZString& lhs, char rhs);
    RAZIX_API RZString operator+(char lhs, const RZString& rhs);

    //-----------------------------------------------------------------------------
    // STRING UTILITIES
    //-----------------------------------------------------------------------------

    template<typename T>
    static RZString rz_to_string(const T& value)
    {
        char buffer[128];

        if constexpr (rz_is_same_v<T, RZString>)
            return value;
        else if constexpr (rz_is_same_v<T, const char*>)
            return RZString(value);
        else if constexpr (rz_is_same_v<T, char*>)
            return RZString(value);
        else if constexpr (rz_is_pointer_v<T>)
            rz_snprintf(buffer, sizeof(buffer), "%p", static_cast<const void*>(value));
        else if constexpr (rz_is_integral_v<T> && rz_is_signed_v<T>)
            rz_snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(value));
        else if constexpr (rz_is_integral_v<T> && rz_is_unsigned_v<T>)
            rz_snprintf(buffer, sizeof(buffer), "%u", static_cast<unsigned int>(value));
        else if constexpr (rz_is_floating_point_v<T>)
            rz_snprintf(buffer, sizeof(buffer), "%f", static_cast<double>(value));
        else
            rz_snprintf(buffer, sizeof(buffer), "<unhandled:%s>", typeid(T).name());

        return RZString(buffer);
    }

    // Hash specialization for RZString to use with RZHashMap
    template<>
    struct rz_hash<RZString>
    {
        size_t operator()(const RZString& str) const
        {
            return static_cast<size_t>(str.hash());
        }
    };

}    // namespace Razix

namespace std {
    template<>
    struct hash<Razix::RZString>
    {
        size_t operator()(const Razix::RZString& str) const
        {
            return str.hash();
        }
    };
}    // namespace std

#ifndef RAZIX_GOLD_MASTER

    #include <spdlog/spdlog.h>    // only needed for fmt integration

namespace fmt {
    template<>
    struct formatter<Razix::RZString>
    {
        template<typename ParseContext>
        constexpr auto parse(ParseContext& ctx)
        {
            return ctx.begin();
        }

        template<typename FormatContext>
        auto format(const Razix::RZString& str, FormatContext& ctx)
        {
            return fmt::format_to(ctx.out(), "{}", str.c_str());
        }
    };
}    // namespace fmt

#endif    // RAZIX_GOLD_MASTER
#endif    // _RZ_STRING_H_
