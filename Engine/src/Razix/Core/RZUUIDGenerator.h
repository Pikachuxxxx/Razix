#pragma once

#include "Razix/Core/RZUUID.h"

// TODO: Use only a single RZUUID constructor to create a UUID successfully

namespace Razix {

    /**
     * Generates UUIDv4 from a provided random generator (c++11 <random> module)
     * std::mt19937_64 is highly recommended as it has a SIMD implementation that
     * makes it very fast and it produces high quality randomness.
     */
    class RZUUIDGenerator
    {
    public:
        RZUUIDGenerator() {}

        static RZUUID generateUUID()
        {
            std::shared_ptr<std::mt19937_64> generator = std::make_shared<std::mt19937_64>(std::random_device()());
            std::uniform_int_distribution<uint64_t> distribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
            // The two masks set the uuid version (4) and variant (1)
            const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
            const __m128i or_mask = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
            __m128i n = _mm_set_epi64x(distribution(*generator), distribution(*generator));
            __m128i uuid = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);

            return RZUUID(uuid);
        }
             
    private:
        /*       std::shared_ptr<std::mt19937_64> generator;
               std::uniform_int_distribution<uint64_t> distribution;*/
    };
}

