#pragma once

#include "Razix/Core/RZUUID.h"


namespace Razix {

    /**
     * Generates UUIDv4 from a provided random generator (c++11 <random> module)
     * std::mt19937_64 is highly recommended as it has a SIMD implementation that
     * makes it very fast and it produces high quality randomness.
     */
    template <typename RNG>
    class RZUUIDGenerator
    {
    public:
        RZUUIDGenerator() : generator(new RNG(std::random_device()())), distribution(0, 18446744073709551615)
        {}

        RZUUID generateUUID()
        {
            // The two masks set the uuid version (4) and variant (1)
            const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
            const __m128i or_mask = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
            __m128i n = _mm_set_epi64x(distribution(*generator), distribution(*generator));
            __m128i uuid = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);

            return RZUUID(uuid);
        }
             
    private:
        std::shared_ptr<RNG> generator;
        std::uniform_int_distribution<uint64_t> distribution;
    };
}

