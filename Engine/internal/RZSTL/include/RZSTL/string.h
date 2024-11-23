#ifndef RZSTL_STRING_H
#define RZSTL_STRING_H

#include "RZSTL/config.h"

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {

#if USE_EASTL
        using string = eastl::string;
#else

        static constexpr bool str_less(const char* lhs, const char* rhs) noexcept
        {
            return *lhs && *rhs && *lhs == *rhs ? str_less(lhs + 1, rhs + 1) : *lhs < *rhs;
        }

        static constexpr bool str_equal(const char* lhs, const char* rhs) noexcept
        {
            return *lhs == *rhs && (*lhs == '\0' || str_equal(lhs + 1, rhs + 1));
        }

        // Use different constants for 32 bit vs. 64 bit size_t
        constexpr std::size_t hash_offset =
            std::conditional_t<sizeof(std::size_t) < 8,
                std::integral_constant<uint32_t, 0x811C9DC5>,
                std::integral_constant<uint64_t, 0xCBF29CE484222325> >::value;
        constexpr std::size_t hash_prime =
            std::conditional_t<sizeof(std::size_t) < 8,
                std::integral_constant<uint32_t, 0x1000193>,
                std::integral_constant<uint64_t, 0x100000001B3> >::value;

        // FNV-1a hash
        constexpr static std::size_t str_hash(const char* str,
            const std::size_t                             value = hash_offset) noexcept
        {
            return *str ? str_hash(str + 1, (value ^ static_cast<std::size_t>(*str)) * hash_prime) : value;
        }

        class string
        {
        private:
            const char* data;

        public:
            constexpr string(char const* data) noexcept
                : data(data)
            {
            }

            constexpr string(const string&) noexcept            = default;
            constexpr string(string&&) noexcept                 = default;
            constexpr string& operator=(const string&) noexcept = default;
            constexpr string& operator=(string&&) noexcept      = default;

            constexpr bool operator<(const string& rhs) const noexcept
            {
                return str_less(data_, rhs.data_);
            }

            constexpr bool operator==(const string& rhs) const noexcept
            {
                return str_equal(data_, rhs.data_);
            }

            constexpr const char* data() const noexcept
            {
                return data;
            }

            constexpr const char* c_str() const noexcept
            {
                return data;
            }
        };

        template<>
        struct hash< ::rzstl::string>
        {
            constexpr std::size_t operator()(const ::rzstl::string& str) const
            {
                return str_hash(str.data());
            }
#endif
    }    // namespace rzstl
}    // namespace Razix

#endif
