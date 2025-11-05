#pragma once

#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/RZDataTypes.h"

namespace Razix {

    template<typename T>
    inline constexpr T flipBits(T val)
    {
        static_assert(rz_is_integral_v<T>, "flipBits requires an integral type");
        int x = 1;
        while (x <= val)
            x <<= 1;
        x--;    // mask is usually N - 1
        return x ^ val;
    }

    template<typename T>
    inline constexpr T toggleBit(T value, uint8_t bitIndex)
    {
        static_assert(rz_is_integral_v<T>, "toggleBit requires an integral type");
        using UnsignedT = rz_make_unsigned_t<T>;
        return value ^ static_cast<T>(UnsignedT(1) << bitIndex);
    }

    template<typename T>
    inline constexpr bool isBitSet(T value, uint8_t bitIndex)
    {
        static_assert(rz_is_integral_v<T>, "isBitSet requires an integral type");
        using UnsignedT = rz_make_unsigned_t<T>;
        return (static_cast<UnsignedT>(value) & (UnsignedT(1) << bitIndex)) != 0;
    }

    template<typename T>
    inline constexpr T setBit(T value, uint8_t bitIndex)
    {
        static_assert(rz_is_integral_v<T>, "setBit requires an integral type");
        using UnsignedT = rz_make_unsigned_t<T>;
        return value | static_cast<T>(UnsignedT(1) << bitIndex);
    }

    template<typename T>
    inline constexpr T clearBit(T value, uint8_t bitIndex)
    {
        static_assert(rz_is_integral_v<T>, "clearBit requires an integral type");
        using UnsignedT = rz_make_unsigned_t<T>;
        return value & static_cast<T>(~(UnsignedT(1) << bitIndex));
    }
}    // namespace Razix
