#ifndef _RZ_STRING_H_
#define _RZ_STRING_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

// get ready for some convoluted shit!
// we are including string.h for memcpy, unless we have
// our own rz_memcpy handwritten using assembly per platform we do this, sorry!
// We need to copy at word granularity using SIMD instuctions like vmovdq etc.
#include <string.h>    // for memcpy only

#define RAZIX_SSO_STRING_SIZE 64

namespace Razix {

    // [header declarations taken from]: https://en.cppreference.com/w/cpp/string/basic_string.html
    class RAZIX_API RZString
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

        RZString() = default;
        ~RZString()
        {
            if (m_is_using_heap)
                Memory::RZFree(m_data.ptr);
        }

        RZString(const RZString& other)
        {
            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                char* ptr  = (char*) Memory::RZMalloc(m_length);
                m_data.ptr = ptr;
            } else {
                // copy string as-is if it's SSO
                memcpy(m_data.sso, other.m_data.sso, RAZIX_SSO_STRING_SIZE);
            }
        }

        RZString& operator=(const RZString& other)
        {
            if (*this == other)
                return *this;

            if (m_is_using_heap)
                Memory::RZFree(m_data.ptr);

            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                char* ptr  = (char*) Memory::RZMalloc(m_length);
                m_data.ptr = ptr;
            } else {
                // copy string as-is if it's SSO
                memcpy(m_data.sso, other.m_data.sso, RAZIX_SSO_STRING_SIZE);
            }

            return *this;
        }

        RZString(RZString&& other) noexcept
        {
            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr       = other.m_data.ptr;
                other.m_data.ptr = NULL;
            } else {
                memcpy(m_data.sso, other.m_data.sso, m_length);
            }

            other.m_is_using_heap = false;
            other.m_length        = 0;
            other.m_capacity      = 0;
        }
        RZString& operator=(RZString&& other) noexcept
        {
            if (*this == other)
                return *this;

            if (m_data.ptr && m_is_using_heap)
                Memory::RZFree(m_data.ptr);

            m_length        = other.m_length;
            m_capacity      = other.m_capacity;
            m_is_using_heap = other.m_is_using_heap;

            if (m_is_using_heap) {
                m_data.ptr       = other.m_data.ptr;
                other.m_data.ptr = NULL;
            } else {
                memcpy(m_data.sso, other.m_data.sso, m_length);
            }

            other.m_is_using_heap = false;
            other.m_length        = 0;
            other.m_capacity      = 0;

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
        void clear();

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

        u32 compare(const RZString& str) const;
        u32 compare(sz pos1, sz count1, const RZString& str) const;
        u32 compare(sz pos1, sz count1, const RZString& str, sz pos2, sz count2) const;
        u32 compare(const char* str) const;
        u32 compare(sz pos1, sz count1, const char* str) const;
        u32 compare(sz pos1, sz count1, const char* str, sz count2) const;

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
        sz   m_length        = 0;
        sz   m_capacity      = RAZIX_SSO_STRING_SIZE;
        bool m_is_using_heap = false;

        union
        {
            char  sso[RAZIX_SSO_STRING_SIZE];
            char* ptr;
        } m_data;

        friend RZString operator+(const RZString& lhs, const RZString& rhs);
        friend RZString operator+(const RZString& lhs, const char* rhs);
        friend RZString operator+(const char* lhs, const RZString& rhs);
        friend RZString operator+(const RZString& lhs, char rhs);
        friend RZString operator+(char lhs, const RZString& rhs);
    };

    RZString operator+(const RZString& lhs, const RZString& rhs);
    RZString operator+(const RZString& lhs, const char* rhs);
    RZString operator+(const char* lhs, const RZString& rhs);
    RZString operator+(const RZString& lhs, char rhs);
    RZString operator+(char lhs, const RZString& rhs);

}    // namespace Razix

#endif    // _RZ_STRING_H_
