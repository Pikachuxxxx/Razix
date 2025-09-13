#pragma once

#include <type_traits>    // is_integral_v

namespace Razix {
    namespace rzstl {
        namespace bitmanip {

            // XOR is used to flip bits

            template<typename T>
            inline constexpr T flipBits(T val)
            {
                static_assert(std::is_integral_v<T>, "flipBits requires an integral type");
                int x = 1;
                while (x <= val)
                    x <<= 1;
                x--;
                return x ^ val;
            }

            template<typename T>
            inline constexpr T toggleBit(T value, uint8_t bitIndex)
            {
                static_assert(std::is_integral_v<T>, "toggleBit requires an integral type");
                using UnsignedT = std::make_unsigned_t<T>;
                return value ^ static_cast<T>(UnsignedT(1) << bitIndex);
            }

            template<typename T>
            inline constexpr bool isBitSet(T value, uint8_t bitIndex)
            {
                static_assert(std::is_integral_v<T>, "isBitSet requires an integral type");
                using UnsignedT = std::make_unsigned_t<T>;
                return (static_cast<UnsignedT>(value) & (UnsignedT(1) << bitIndex)) != 0;
            }

            template<typename T>
            inline constexpr T setBit(T value, uint8_t bitIndex)
            {
                static_assert(std::is_integral_v<T>, "setBit requires an integral type");
                using UnsignedT = std::make_unsigned_t<T>;
                return value | static_cast<T>(UnsignedT(1) << bitIndex);
            }

            template<typename T>
            inline constexpr T clearBit(T value, uint8_t bitIndex)
            {
                static_assert(std::is_integral_v<T>, "clearBit requires an integral type");
                using UnsignedT = std::make_unsigned_t<T>;
                return value & static_cast<T>(~(UnsignedT(1) << bitIndex));
            }

        }    // namespace bitmanip
    }    // namespace rzstl
}    // namespace Razix