#include "rzxpch.h"
//#include "RZUUIDGenerator.h"

//namespace Razix {
//    template <typename RNG>
//    RZUUID Razix::RZUUIDGenerator<RNG>::generateUUID()
//    {
//        // The two masks set the uuid version (4) and variant (1)
//        const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
//        const __m128i or_mask = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
//        __m128i n = _mm_set_epi64x(distribution(*generator), distribution(*generator));
//        __m128i uuid = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);
//
//        return RZUUID(uuid);
//    }
//}