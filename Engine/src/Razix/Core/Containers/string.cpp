// clang-format off
#include "rzxpch.h"
// clang-format on
#include "string.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <DbgHelp.h>
    #include <windows.h>
#else
    #include <cxxabi.h>    // __cxa_demangle()
#endif

namespace Razix {

    static sz rz_strlen(const char* str)
    {
        RAZIX_ASSERT(str != NULL, "rz_strlen() null pointer");

        sz len = 0;
        while (str[len] != '\0') {
            len++;
        }
        return len;
    }

    static i32 rz_strcmp(const char* s1, const char* s2)
    {
        RAZIX_ASSERT(s1 != NULL, "rz_strcmp() s1 is null");
        RAZIX_ASSERT(s2 != NULL, "rz_strcmp() s2 is null");

        while (*s1 && *s2 && *s1 == *s2) {
            s1++;
            s2++;
        }
        return (i32) (*s1 - *s2);
    }

    static i32 rz_memcmp(const void* ptr1, const void* ptr2, sz count)
    {
        RAZIX_ASSERT(ptr1 != NULL, "rz_memcmp() ptr1 is null");
        RAZIX_ASSERT(ptr2 != NULL, "rz_memcmp() ptr2 is null");

        const unsigned char* p1 = (const unsigned char*) ptr1;
        const unsigned char* p2 = (const unsigned char*) ptr2;

        for (sz i = 0; i < count; i++) {
            if (p1[i] != p2[i]) {
                return (i32) (p1[i] - p2[i]);
            }
        }
        return 0;
    }

    RZString::RZString(const char* str)
    {
        if (!str) {
            m_data.sso[0] = '\0';
            return;
        }

        size_t len = rz_strlen(str);
        m_length   = len;
        if (len < RAZIX_SSO_STRING_SIZE) {
            memcpy(m_data.sso, str, len);
            m_data.sso[len] = '\0';
        } else {
            m_capacity = len + 1;
            m_data.ptr = static_cast<char*>(Memory::RZMalloc(len + 1));
            memcpy(m_data.ptr, str, len);
            m_data.ptr[len] = '\0';
            m_is_using_heap = true;
        }
    }

    RZString::RZString(const char* str, sz count)
    {
        if (!str || count == 0) {
            m_data.sso[0] = '\0';
            return;
        }

        m_length = count;
        if (count < RAZIX_SSO_STRING_SIZE) {
            memcpy(m_data.sso, str, count);
            m_data.sso[count] = '\0';
        } else {
            m_capacity = count + 1;
            m_data.ptr = static_cast<char*>(Memory::RZMalloc(count + 1));
            memcpy(m_data.ptr, str, count);
            m_data.ptr[count] = '\0';
            m_is_using_heap   = true;
        }
    }

    RZString::RZString(const RZString& other, sz pos, sz count /* = npos */)
    {
        const char* src       = other.c_str();
        sz          other_len = other.length();

        if (pos >= other_len) {
            m_data.sso[0] = '\0';
            return;
        }

        sz len   = (count == npos || pos + count > other_len)
                       ? (other_len - pos)
                       : count;
        m_length = len;
        if (len < RAZIX_SSO_STRING_SIZE) {
            memcpy(m_data.sso, src + pos, len);
            m_data.sso[len] = '\0';
        } else {
            m_capacity = len + 1;
            m_data.ptr = static_cast<char*>(Memory::RZMalloc(len + 1));
            memcpy(m_data.ptr, src + pos, len);
            m_data.ptr[len] = '\0';
            m_is_using_heap = true;
        }
    }

    RZString::RZString(sz count, char ch)
    {
        if (count == 0) {
            m_data.sso[0] = '\0';
            return;
        }
        m_length = count;
        if (count < RAZIX_SSO_STRING_SIZE) {
            memset(m_data.sso, ch, count);
            m_data.sso[count] = '\0';
        } else {
            m_capacity = count + 1;
            m_data.ptr = static_cast<char*>(Memory::RZMalloc(count + 1));
            memset(m_data.ptr, ch, count);
            m_data.ptr[count] = '\0';
            m_is_using_heap   = true;
        }
    }

    RZString::reference RZString::at(sz pos)
    {
        RAZIX_ASSERT(m_length > 0, "RZString::at() called on empty string");
        RAZIX_ASSERT(pos < m_length, "RZString::at() out of range");
        RAZIX_ASSERT(m_capacity > 0, "RZString::at() invalid capacity");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::at() null heap pointer");

        return m_is_using_heap ? m_data.ptr[pos] : m_data.sso[pos];
    }

    RZString::const_reference RZString::at(sz pos) const
    {
        RAZIX_ASSERT(m_length > 0, "RZString::at() called on empty string");
        RAZIX_ASSERT(pos < m_length, "RZString::at() out of range");
        RAZIX_ASSERT(m_capacity > 0, "RZString::at() invalid capacity");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::at() null heap pointer");

        return m_is_using_heap ? m_data.ptr[pos] : m_data.sso[pos];
    }

    RZString::reference RZString::operator[](sz pos)
    {
        // No exception semantics here, just debug checks
        RAZIX_ASSERT(pos < m_length, "RZString::operator[] out of range");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::operator[] null heap pointer");

        return m_is_using_heap ? m_data.ptr[pos] : m_data.sso[pos];
    }

    RZString::const_reference RZString::operator[](sz pos) const
    {
        RAZIX_ASSERT(pos < m_length, "RZString::operator[] out of range");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::operator[] null heap pointer");

        return m_is_using_heap ? m_data.ptr[pos] : m_data.sso[pos];
    }

    RZString::reference RZString::front()
    {
        RAZIX_ASSERT(m_length > 0, "RZString::front() called on empty string");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::front() null heap pointer");

        return m_is_using_heap ? m_data.ptr[0] : m_data.sso[0];
    }

    RZString::const_reference RZString::front() const
    {
        RAZIX_ASSERT(m_length > 0, "RZString::front() called on empty string");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::front() null heap pointer");

        return m_is_using_heap ? m_data.ptr[0] : m_data.sso[0];
    }

    RZString::reference RZString::back()
    {
        RAZIX_ASSERT(m_length > 0, "RZString::back() called on empty string");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::back() null heap pointer");
        RAZIX_ASSERT(m_length <= m_capacity, "RZString::back() invalid length");

        return m_is_using_heap ? m_data.ptr[m_length - 1] : m_data.sso[m_length - 1];
    }

    RZString::const_reference RZString::back() const
    {
        RAZIX_ASSERT(m_length > 0, "RZString::back() called on empty string");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::back() null heap pointer");
        RAZIX_ASSERT(m_length <= m_capacity, "RZString::back() invalid length");

        return m_is_using_heap ? m_data.ptr[m_length - 1] : m_data.sso[m_length - 1];
    }

    RZString::pointer RZString::data()
    {
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::data() null heap pointer");
        return m_is_using_heap ? m_data.ptr : m_data.sso;
    }

    RZString::const_pointer RZString::data() const
    {
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::data() null heap pointer");
        return m_is_using_heap ? m_data.ptr : m_data.sso;
    }

    const char* RZString::c_str() const
    {
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::c_str() null heap pointer");
        return m_is_using_heap ? m_data.ptr : m_data.sso;
    }

    RZString::pointer RZString::begin()
    {
        return m_is_using_heap ? m_data.ptr : m_data.sso;
    }

    RZString::const_pointer RZString::begin() const
    {
        return m_is_using_heap ? m_data.ptr : m_data.sso;
    }

    RZString::pointer RZString::end()
    {
        return (m_is_using_heap ? m_data.ptr : m_data.sso) + m_length;
    }

    RZString::const_pointer RZString::end() const
    {
        return (m_is_using_heap ? m_data.ptr : m_data.sso) + m_length;
    }

    bool RZString::empty() const
    {
        return m_length == 0;
    }
    sz RZString::size() const
    {
        return m_length;
    }
    sz RZString::length() const
    {
        return m_length;
    }
    sz RZString::capacity() const
    {
        return m_capacity;
    }
    sz RZString::max_size() const
    {
        return m_capacity;
    }

    void RZString::reserve(sz new_capacity)
    {
        RAZIX_ASSERT(new_capacity > 0, "RZString::reserve() capacity must be > 0");
        RAZIX_ASSERT(new_capacity >= m_length, "RZString::reserve() new capacity smaller than length");

        if (new_capacity <= m_capacity) return;

        if (new_capacity <= RAZIX_SSO_STRING_SIZE) {
            m_capacity = RAZIX_SSO_STRING_SIZE;
            return;
        }

        // Growth strategy: always at least double
        while (m_capacity < new_capacity)
            m_capacity *= 2;

        char* new_buf = (char*) Memory::RZMalloc(m_capacity + 1);
        char* src     = m_is_using_heap ? m_data.ptr : m_data.sso;

        memcpy(new_buf, src, m_length);
        new_buf[m_length] = '\0';
        memset(new_buf + m_length, 0xCD, m_capacity - m_length);

        if (m_is_using_heap)
            Memory::RZFree(m_data.ptr);

        m_data.ptr      = new_buf;
        m_is_using_heap = true;
    }

    void RZString::resize(sz new_length)
    {
        if (new_length > m_capacity) {
            reserve(new_length + 1);
        }
        m_length = new_length;

        char* base       = (m_is_using_heap ? m_data.ptr : m_data.sso);
        base[new_length] = '\0';
    }

    void RZString::clear()
    {
        if (m_length == 0)
            return;

        RAZIX_ASSERT(m_capacity > 0, "RZString::clear() invalid capacity");
        RAZIX_ASSERT(!m_is_using_heap || m_data.ptr != NULL, "RZString::clear() null heap pointer");

        // Reset only logical length — don't free memory
        if (m_is_using_heap)
            m_data.ptr[0] = '\0';
        else
            m_data.sso[0] = '\0';

        m_length = 0;
    }

    void RZString::setLength(sz length)
    {
        m_length = length;
    }

    RZString& RZString::append(const RZString& str)
    {
        return append(str.c_str(), str.m_length);
    }

    RZString& RZString::append(const RZString& str, sz pos, sz count)
    {
        RAZIX_ASSERT(pos <= str.m_length, "RZString::append() pos out of range");
        if (count == npos || pos + count > str.m_length)
            count = str.m_length - pos;
        return append(str.c_str() + pos, count);
    }

    RZString& RZString::append(const char* str)
    {
        RAZIX_ASSERT(str != NULL, "RZString::append() null pointer");
        return append(str, rz_strlen(str));
    }

    RZString& RZString::append(const char* str, sz count)
    {
        RAZIX_ASSERT(str != NULL, "RZString::append() null pointer");
        reserve(m_length + count + 1);    // +1 for \0

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memcpy(dst + m_length, str, count);
        m_length += count;
        dst[m_length] = '\0';
        return *this;
    }

    RZString& RZString::append(sz count, char ch)
    {
        reserve(m_length + count + 1);    // +1 for \0

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memset(dst + m_length, ch, count);
        m_length += count;
        dst[m_length] = '\0';
        return *this;
    }

    RZString& RZString::insert(sz pos, const RZString& str)
    {
        return insert(pos, str.c_str(), str.m_length);
    }

    RZString& RZString::insert(sz pos, const char* str)
    {
        return insert(pos, str, rz_strlen(str));
    }

    RZString& RZString::insert(sz pos, sz count, char ch)
    {
        RAZIX_ASSERT(pos <= m_length, "RZString::insert() pos out of range");

        reserve(m_length + count + 1);    // +1 for \0

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memmove(dst + pos + count, dst + pos, m_length - pos);
        memset(dst + pos, ch, count);
        m_length += count;
        dst[m_length] = '\0';

        return *this;
    }

    RZString& RZString::insert(sz pos, const char* str, sz count)
    {
        RAZIX_ASSERT(pos <= m_length, "RZString::insert() pos out of range");
        RAZIX_ASSERT(str != NULL, "RZString::insert() null pointer");

        reserve(m_length + count + 1);    // +1 for \0

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memmove(dst + pos + count, dst + pos, m_length - pos);
        memcpy(dst + pos, str, count);
        m_length += count;
        dst[m_length] = '\0';

        return *this;
    }

    RZString& RZString::erase(sz pos, sz count)
    {
        RAZIX_ASSERT(pos <= m_length, "RZString::erase() pos out of range");

        if (count == npos || pos + count > m_length)
            count = m_length - pos;

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memmove(dst + pos, dst + pos + count, m_length - pos - count + 1);
        m_length -= count;

        return *this;
    }

    RZString& RZString::replace(sz pos, sz count, const RZString& str)
    {
        RAZIX_ASSERT(pos <= m_length, "RZString::replace() pos out of range");
        RAZIX_ASSERT(str.c_str() != NULL, "RZString::replace() null pointer");

        return replace(pos, count, str.c_str());
    }

    RZString& RZString::replace(sz pos, sz count, const char* str)
    {
        if (count == npos || pos + count > m_length)
            count = m_length - pos;

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memcpy(dst + pos, str, count);

        return *this;
    }

    RZString& RZString::replace(sz pos, sz count, char ch)
    {
        if (count == npos || pos + count > m_length)
            count = m_length - pos;

        char* dst = m_is_using_heap ? m_data.ptr : m_data.sso;
        memset(dst + pos, ch, count);

        return *this;
    }

    RZString& RZString::push_back(char ch)
    {
        RAZIX_ASSERT(m_length < m_capacity, "RZString::push_back() out of range");

        char* dst       = m_is_using_heap ? m_data.ptr : m_data.sso;
        dst[m_length++] = ch;
        dst[m_length]   = '\0';

        return *this;
    }
    RZString& RZString::pop_back()
    {
        RAZIX_ASSERT(m_length > 0, "RZString::pop_back() called on empty string");

        char* dst       = m_is_using_heap ? m_data.ptr : m_data.sso;
        dst[--m_length] = '\0';

        if (m_is_using_heap && m_length < RAZIX_SSO_STRING_SIZE) {
            // move data from heap to SSO buffer
            memcpy(m_data.sso, dst, m_length);
            Memory::RZFree(dst);
            m_is_using_heap = false;
        }

        return *this;
    }

    void RZString::swap(RZString& other)
    {
        RZString temp = other;
        other         = *this;
        *this         = temp;
    }

    sz RZString::find(const RZString& str, sz pos /*  = 0 */) const
    {
        return find(str.c_str(), pos);
    }

    sz RZString::find(const char* str, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");
        RAZIX_ASSERT(str != NULL, "str is NULL");
        RAZIX_ASSERT(rz_strlen(str) > 0, "search string is empty");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* dst     = base + pos;
        const char* sub     = strchr(dst, str[0]);
        sz          str_len = rz_strlen(str);

        while (sub != NULL) {
            if (rz_memcmp(sub, str, str_len) == 0) {
                return (sub - base);
            }
            dst = sub + 1;
            sub = strchr(dst, str[0]);
        }

        return npos;
    }

    sz RZString::find(char ch, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* dst  = base + pos;
        const char* sub  = strchr(dst, ch);

        if (sub == NULL) {
            return npos;
        }

        return (sub - base);
    }

    sz RZString::rfind(const RZString& str, sz pos /*  = npos */) const
    {
        return rfind(str.c_str(), pos);
    }

    sz RZString::rfind(const char* str, sz pos /*  = npos */) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");
        RAZIX_ASSERT(rz_strlen(str) > 0, "search string is empty");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        sz          str_len = rz_strlen(str);

        if (pos == npos || pos > m_length) {
            pos = m_length;
        }

        if (str_len > pos) {
            return npos;
        }

        const char* start = base + pos - str_len;

        while (start >= base) {
            if (rz_memcmp(start, str, str_len) == 0) {
                return (start - base);
            }
            start--;
        }

        return npos;
    }

    sz RZString::rfind(char ch, sz pos /*  = npos */) const
    {
        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (pos == npos || pos >= m_length) {
            pos = m_length - 1;
        }

        const char* start = base + pos;

        while (start >= base) {
            if (*start == ch) {
                return (start - base);
            }
            start--;
        }

        return npos;
    }

    sz RZString::find_first_of(const RZString& str, sz pos /*  = 0 */) const
    {
        return find_first_of(str.c_str(), pos);
    }

    sz RZString::find_first_of(const char* str, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        // if empty string we reached end
        if (!m_length) return npos;

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* current = base + pos;
        sz          str_len = rz_strlen(str);

        while (*current != '\0') {
            for (sz i = 0; i < str_len; i++) {
                if (*current == str[i]) {
                    return (current - base);
                }
            }
            current++;
        }

        return npos;
    }

    sz RZString::find_first_of(char ch, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* sub  = strchr(base + pos, ch);

        if (sub == NULL) {
            return npos;
        }

        return (sub - base);
    }

    sz RZString::find_last_of(const RZString& str, sz pos /*  = npos */) const
    {
        return find_last_of(str.c_str(), pos);
    }

    sz RZString::find_last_of(const char* str, sz pos /*  = npos */) const
    {
        RAZIX_ASSERT(m_length > 0, "find_last_of() called on empty string");
        RAZIX_ASSERT(str != NULL, "str is NULL");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        sz          str_len = rz_strlen(str);

        if (pos == npos || pos >= m_length) {
            pos = m_length - 1;
        }

        const char* start = base + pos;

        while (start >= base) {
            for (sz i = 0; i < str_len; i++) {
                if (*start == str[i]) {
                    return (start - base);
                }
            }
            start--;
        }

        return npos;
    }

    sz RZString::find_last_of(char ch, sz pos /*  = npos */) const
    {
        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (pos == npos || pos >= m_length) {
            pos = m_length - 1;
        }

        const char* start = base + pos;

        while (start >= base) {
            if (*start == ch) {
                return (start - base);
            }
            start--;
        }

        return npos;
    }

    sz RZString::find_first_not_of(const RZString& str, sz pos /*  = 0 */) const
    {
        return find_first_not_of(str.c_str(), pos);
    }

    sz RZString::find_first_not_of(const char* str, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* current = base + pos;
        sz          str_len = rz_strlen(str);

        while (*current != '\0') {
            u8 found = 0;
            for (sz i = 0; i < str_len; i++) {
                if (*current == str[i]) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                return (current - base);
            }
            current++;
        }

        return npos;
    }

    sz RZString::find_first_not_of(char ch, sz pos /*  = 0 */) const
    {
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* current = base + pos;

        while (*current != '\0') {
            if (*current != ch) {
                return (current - base);
            }
            current++;
        }

        return npos;
    }

    sz RZString::find_last_not_of(const RZString& str, sz pos /*  = npos */) const
    {
        return find_last_not_of(str.c_str(), pos);
    }

    sz RZString::find_last_not_of(const char* str, sz pos /*  = npos */) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");

        const char* base    = (m_is_using_heap ? m_data.ptr : m_data.sso);
        sz          str_len = rz_strlen(str);

        if (pos == npos || pos >= m_length) {
            pos = m_length - 1;
        }

        const char* start = base + pos;

        while (start >= base) {
            u8 found = 0;
            for (sz i = 0; i < str_len; i++) {
                if (*start == str[i]) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                return (start - base) + 1;
            }
            start--;
        }

        return npos;
    }

    sz RZString::find_last_not_of(char ch, sz pos /*  = npos */) const
    {
        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (pos == npos || pos >= m_length) {
            pos = m_length - 1;
        }

        const char* start = base + pos;

        while (start >= base) {
            if (*start != ch) {
                return (start - base) + 1;
            }
            start--;
        }

        return npos;
    }

    RZString RZString::substr(sz pos /*  = 0 */, sz count /*  = npos */) const
    {
        RAZIX_ASSERT(pos <= m_length, "pos out of bounds");

        // if empty string we reached end
        if (!m_length) {
            return RZString();
        }

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (count == npos) {
            count = m_length - pos;
        }

        if (pos + count > m_length) {
            count = m_length - pos;
        }

        RZString result;
        if (count > 0) {
            result.append(base + pos, count);
        }

        return result;
    }

    i32 RZString::compare(const RZString& str) const
    {
        RAZIX_ASSERT(str.m_length > 0, "compare() with empty string");

        return compare((const char*) str.c_str());
    }

    i32 RZString::compare(sz pos1, sz count1, const RZString& str) const
    {
        RAZIX_ASSERT(pos1 <= m_length, "pos1 out of bounds");
        RAZIX_ASSERT(str.m_length > 0, "compare() with empty string");

        return compare(pos1, count1, (const char*) str.c_str(), str.m_length);
    }

    i32 RZString::compare(sz pos1, sz count1, const RZString& str, sz pos2, sz count2) const
    {
        RAZIX_ASSERT(pos1 <= m_length, "pos1 out of bounds");
        RAZIX_ASSERT(str.m_length > 0, "compare() with empty string");
        RAZIX_ASSERT(pos2 <= str.m_length, "pos2 out of bounds");

        const char* base1 = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* base2 = (str.m_is_using_heap ? str.c_str() : str.m_data.sso);

        if (count1 == npos) {
            count1 = m_length - pos1;
        }
        if (count2 == npos) {
            count2 = str.m_length - pos2;
        }

        if (pos1 + count1 > m_length) {
            count1 = m_length - pos1;
        }
        if (pos2 + count2 > str.m_length) {
            count2 = str.m_length - pos2;
        }

        sz  min_count = (count1 < count2) ? count1 : count2;
        i32 result    = rz_memcmp(base1 + pos1, base2 + pos2, min_count);

        if (result != 0) {
            return result;
        }

        return (count1 > count2) ? 1 : (count1 < count2) ? -1
                                                         : 0;
    }

    i32 RZString::compare(sz pos1, sz count1, const char* str) const
    {
        RAZIX_ASSERT(pos1 <= m_length, "pos1 out of bounds");
        RAZIX_ASSERT(str != NULL, "str is NULL");

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (count1 == npos) {
            count1 = m_length - pos1;
        }
        if (pos1 + count1 > m_length) {
            count1 = m_length - pos1;
        }

        sz  str_len   = rz_strlen(str);
        sz  min_count = (count1 < str_len) ? count1 : str_len;
        i32 result    = rz_memcmp(base + pos1, str, min_count);

        if (result != 0) {
            return result;
        }

        return (count1 > str_len) ? 1 : (count1 < str_len) ? -1
                                                           : 0;
    }

    i32 RZString::compare(sz pos1, sz count1, const char* str, sz count2) const
    {
        RAZIX_ASSERT(pos1 <= m_length, "pos1 out of bounds");
        RAZIX_ASSERT(str != NULL, "str is NULL");

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);

        if (count1 == npos) {
            count1 = m_length - pos1;
        }
        if (pos1 + count1 > m_length) {
            count1 = m_length - pos1;
        }

        sz str_len = rz_strlen(str);
        if (count2 == npos) {
            count2 = str_len;
        }
        if (count2 > str_len) {
            count2 = str_len;
        }

        sz  min_count = (count1 < count2) ? count1 : count2;
        i32 result    = rz_memcmp(base + pos1, str, min_count);

        if (result != 0) {
            return result;
        }

        return (count1 > count2) ? 1 : (count1 < count2) ? -1
                                                         : 0;
    }

    i32 RZString::compare(const char* str) const
    {
        RAZIX_ASSERT(str != NULL, "compare() with empty string");

        return rz_strcmp(c_str(), str);
    }

    RZString& RZString::operator=(const char* str)
    {
        *this = RZString(str);
        return *this;
    }
    RZString& RZString::operator=(char ch)
    {
        *this = RZString(1, ch);
        return *this;
    }

    bool RZString::operator==(const RZString& other) const
    {
        if (m_length != other.m_length) {
            return false;
        }

        const char* base1 = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* base2 = (other.m_is_using_heap ? other.m_data.ptr : other.m_data.sso);

        return rz_memcmp(base1, base2, m_length) == 0;
    }

    bool RZString::operator==(const char* str) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");

        const char* base = (m_is_using_heap ? m_data.ptr : m_data.sso);
        return rz_strcmp(base, str) == 0;
    }

    bool RZString::operator!=(const RZString& other) const
    {
        return !(*this == other);
    }

    bool RZString::operator!=(const char* str) const
    {
        RAZIX_ASSERT(str != NULL, "str is NULL");

        return !(*this == str);
    }

    bool RZString::operator<(const RZString& other) const
    {
        const char* base1 = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* base2 = (other.m_is_using_heap ? other.m_data.ptr : other.m_data.sso);

        sz  min_len = (m_length < other.m_length) ? m_length : other.m_length;
        i32 result  = rz_memcmp(base1, base2, min_len);

        if (result != 0) {
            return result < 0;
        }

        return m_length < other.m_length;
    }

    bool RZString::operator<=(const RZString& other) const
    {
        return (*this < other) || (*this == other);
    }

    bool RZString::operator>(const RZString& other) const
    {
        const char* base1 = (m_is_using_heap ? m_data.ptr : m_data.sso);
        const char* base2 = (other.m_is_using_heap ? other.m_data.ptr : other.m_data.sso);

        sz  min_len = (m_length < other.m_length) ? m_length : other.m_length;
        i32 result  = rz_memcmp(base1, base2, min_len);

        if (result != 0) {
            return result > 0;
        }

        return m_length > other.m_length;
    }

    bool RZString::operator>=(const RZString& other) const
    {
        return (*this > other) || (*this == other);
    }

    //--------------------------------------------------------------------
    // Friend operators - for both seperate operations
    //--------------------------------------------------------------------

    RZString operator+(const RZString& lhs, const RZString& rhs)
    {
        RZString result = lhs;
        result.append(rhs);
        return result;
    }

    RZString operator+(const RZString& lhs, const char* rhs)
    {
        RAZIX_ASSERT(rhs != NULL, "operator+() rhs is NULL");

        RZString result = lhs;
        result.append(rhs);
        return result;
    }

    RZString operator+(const char* lhs, const RZString& rhs)
    {
        RAZIX_ASSERT(lhs != NULL, "operator+() lhs is NULL");

        RZString result(lhs);
        result.append(rhs);
        return result;
    }

    RZString operator+(const RZString& lhs, char rhs)
    {
        RZString result = lhs;
        result.append(1, rhs);
        return result;
    }

    RZString operator+(char lhs, const RZString& rhs)
    {
        RZString result;
        result.append(1, lhs);
        result.append(rhs);
        return result;
    }

    //-----------------------------------------------------------------------------
    // STRING UTILITIES
    //-----------------------------------------------------------------------------

    RZString GetFilePathExtension(const RZString& FileName)
    {
        sz pos = FileName.find_last_of('.');
        if (pos != RZString::npos)
            return FileName.substr(pos + 1);
        return RZString();
    }

    RZString RemoveFilePathExtension(const RZString& FileName)
    {
        sz pos = FileName.find_last_of('.');
        if (pos != RZString::npos)
            return FileName.substr(0, pos);
        return FileName;
    }

    RZString GetFileName(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(pos + 1);
        return FilePath;
    }

    RZString GetFileLocation(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(0, pos + 1);
        return FilePath;
    }

    RZString RemoveName(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(0, pos + 1);
        return FilePath;
    }

    bool IsHiddenFile(const RZString& path)
    {
        if (path != RZString("..") && path != RZString(".") && path[0] == '.') {
            return true;
        }

        return false;
    }

    std::vector<RZString> SplitString(const RZString& string, const RZString& delimiters)
    {
        sz start = 0;
        sz end   = string.find_first_of(delimiters);

        std::vector<RZString> result;

        while (end <= RZString::npos) {
            RZString token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == RZString::npos)
                break;

            start = end + 1;
            end   = string.find_first_of(delimiters, start);
        }

        return result;
    }

    std::vector<RZString> SplitString(const RZString& string, char delimiter)
    {
        return SplitString(string, RZString(1, delimiter));
    }

    std::vector<RZString> Tokenize(const RZString& string)
    {
        return SplitString(string, RZString(" \t\n"));
    }

    std::vector<RZString> GetLines(const RZString& string)
    {
        return SplitString(string, RZString("\n"));
    }

    cstr FindToken(cstr str, const RZString& token)
    {
        RAZIX_ASSERT(str != NULL, "FindToken() str is null");

        RZString haystack(str);
        sz       pos = 0;

        while (pos < haystack.length()) {
            pos = haystack.find(token, pos);
            if (pos == RZString::npos)
                return nullptr;

            // Check left boundary
            bool left = (pos == 0) || (pos > 0 && isspace(haystack[pos - 1]));

            // Check right boundary
            sz   end_pos = pos + token.length();
            bool right   = (end_pos >= haystack.length()) || (end_pos < haystack.length() && isspace(haystack[end_pos]));

            if (left && right)
                return haystack.c_str() + pos;

            pos++;
        }

        return nullptr;
    }

    cstr FindToken(const RZString& string, const RZString& token)
    {
        return FindToken(string.c_str(), token);
    }

    i32 FindStringPosition(const RZString& string, const RZString& search, u32 offset)
    {
        RAZIX_ASSERT(offset <= string.length(), "FindStringPosition() offset out of range");

        sz pos = string.find(search, offset);
        if (pos == RZString::npos)
            return -1;
        return static_cast<i32>(pos);
    }

    RZString StringRange(const RZString& string, u32 start, u32 length)
    {
        return string.substr(start, length);
    }

    RZString RemoveStringRange(const RZString& string, u32 start, u32 length)
    {
        RZString result = string;
        return result.erase(start, length);
    }

    RZString TrimWhitespaces(const RZString& string)
    {
        sz start = string.find_first_not_of(RZString(" \t\n\r\f\v"));
        if (start == RZString::npos)
            return RZString();

        sz end = string.find_last_not_of(RZString(" \t\n\r\f\v"));

        return string.substr(start, end - start + 1);
    }

    bool StringContains(const RZString& string, const RZString& chars)
    {
        return string.find(chars) != RZString::npos;
    }

    bool StartsWith(const RZString& string, const RZString& start)
    {
        return string.find(start) == 0;
    }

    i32 NextInt(const RZString& string)
    {
        for (sz i = 0; i < string.size(); i++) {
            if (isdigit(string[i])) {
                // Extract the number starting from position i
                RZString numStr;
                while (i < string.size() && (isdigit(string[i]) || (numStr.empty() && string[i] == '-'))) {
                    numStr.append(1, string[i]);
                    i++;
                }
                return atoi(numStr.c_str());
            }
        }
        return -1;
    }

    bool StringEquals(const RZString& string1, const RZString& string2)
    {
        return string1 == string2;
    }

    RZString StringReplace(RZString str, char ch1, char ch2)
    {
        for (sz i = 0; i < str.length(); ++i) {
            if (str[i] == ch1)
                str[i] = ch2;
        }
        return str;
    }

    RZString StringReplace(RZString str, char ch)
    {
        for (sz i = 0; i < str.length(); ++i) {
            if (str[i] == ch) {
                str = str.substr(0, i) + str.substr(i + 1, str.length());
            }
        }
        return str;
    }

    RZString& BackSlashesToSlashes(RZString& string)
    {
        for (sz i = 0; i < string.length(); i++) {
            if (string[i] == '\\') {
                string[i] = '/';
            }
        }
        return string;
    }

    RZString& SlashesToBackSlashes(RZString& string)
    {
        for (sz i = 0; i < string.length(); i++) {
            if (string[i] == '/') {
                string[i] = '\\';
            }
        }
        return string;
    }

    RZString& RemoveSpaces(RZString& string)
    {
        sz i = 0;
        while (i < string.length()) {
            if (string[i] == ' ') {
                string = string.substr(0, i) + string.substr(i + 1, string.length());
            } else {
                i++;
            }
        }
        return string;
    }

    RZString Demangle(const RZString& string)
    {
        if (string.empty())
            return RZString();

#if defined(RAZIX_PLATFORM_WINDOWS)
        char undecorated_name[1024];
        if (!UnDecorateSymbolName(
                string.c_str(), undecorated_name, sizeof(undecorated_name), UNDNAME_COMPLETE)) {
            return string;
        } else {
            return RZString(undecorated_name);
        }
#else
        char* demangled = nullptr;
        int   status    = -1;
        demangled       = abi::__cxa_demangle(string.c_str(), nullptr, nullptr, &status);
        RZString ret    = status == 0 ? RZString(demangled) : string;
        free(demangled);
        return ret;
#endif
    }

}    // namespace Razix