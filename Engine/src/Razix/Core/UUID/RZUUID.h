#pragma once

#include <random>

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Containers/string.h"

#if defined(RAZIX_APPLE_SILICON) || defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_LINUX_ARM64)
    #include <arm_neon.h>

    #if defined(__clang__) || defined(__GNUC__)
        #define SIMDE_ENABLE_NATIVE_ALIASES
        #define SIMDE_X86_AVX_ENABLE_NATIVE_ALIASES

        #include <simde/arm/neon.h>
        #include <simde/x86/avx.h>     // AVX
        #include <simde/x86/avx2.h>    // AVX
        #include <simde/x86/sse4.1.h>
    #endif
#endif

namespace Razix {

    /**
     * Implementation from : Copyright (c) 2018 Xavier "Crashoz" Launey [https://github.com/crashoz/uuid_v4] MIT license.
     * 
     * "Razix Universally Unique Identifier" or RZUUID. Creates a random V4 style RZUUID as per the format (RFC-4122) described here : https://datatracker.ietf.org/doc/html/rfc4122
     *
     * UUIDs are 128-bit identifiers that can be used to uniquely identify information without requiring central coordination.
     * 
     * The format of an RZUUID hexadecimal string representation is in BNF syntax for bit. [8-4-4-4-12]
     * 
     * RZUUID := time-low - time-mid - time-high-and-version - clock-seq-and-reserved clock-seq-low - node
     * 
     * time-low = 4hexOctet - 8
     * time-mid = 2hexOctet - 4
     * time-high-and-version = 2hexOctet - 4
     * clock-seq-and-reserved = hexOctet - 2 +
     * clock-seq-low = hexOctet - 2 = 4
     * node = 6hexOctet - 12
     * 
     * Note that:
     * hexOctet = hexDigit hexDigit
     * hexDigit = “0” / “1” / “2” / “3” / “4” / “5” / “6” / “7” / “8” / “9” / “a” / “b” / “c” / “d” / “e” / “f” / “A” / “B” / “C” / “D” / “E” / “F”
     * 
     * The version 4 (V4) RZUUID is the most used and is meant for generating UUIDs from truly-random. Except bits 6, 7 and 12 through 15, all other bits are random. The chance of collision are so small that it can be ignored.
     * - the timestamp is a randomly or pseudo-randomly generated 60-bit value
     * - the clock sequence is a randomly or pseudo-randomly generated 14-bit value
     * - the node field is a randomly or pseudo-randomly generated 48-bit
     * Example: version 4 (the 4 in 4e2b) :: 27701bfc-78d0-4e2b-92ca-193cea53fa30
     * 
     * ********************************************************************************************************************
     * 
     * All Razix Engine Entities and Assets will be assigned a RZUUID and this will be used to uniquely identify them, runtime 4entities are also assigned an RZUUID
     * 
     * the number of random version-4 UUIDs which need to be generated in order to have a 50% probability of at least one collision is 2.71 quintillion.
     * Thus, the probability to find a duplicate within 103 trillion version-4 UUIDs is one in a billion.
     */

    class RAZIX_API RZUUID
    {
    public:
        /* Defining the format type of RZUUID (we will use a union with bytes for easier calculation) */
        typedef struct
        {
            u32  time_low;
            u16  time_mid;
            u16  time_hi_and_version;
            u8   clock_seq_hi_and_reserved;
            u8   clock_seq_low;
            char node[6];
        } uuidv4;

    public:
        /* Builds a 128-bits RZUUID */
        RZUUID();
        /* Copy constructor for the RZUUID */
        RZUUID(const RZUUID& other);
        /**
         * Create RZUUID from the 128-bit number using AVX registers
         * 
         * @param uuid 128-bit UUID
         */
        RZUUID(__m128i uuid);
        /**
         * Create RZUUID using 2 64-bit numbers
         * 
         * @param x first number
         * @param y second number
         */
        RZUUID(uint64_t x, uint64_t y);
        /**
         * Creates the RZUUID and 128-bit representation using 8-4-4-4-12 style string characters 
         * 
         * @param bytes bytes representation of the UUID
         */
        RZUUID(const u8* bytes);
        /**
         * Builds an RZUUID from a byte string (16 bytes long)
         * 
         * @param bytes 16byte string representation of the UUID
         */
        explicit RZUUID(const RZString& bytes);

        /* Default destructor */
        ~RZUUID() = default;

        // Static factory methods to parse an RZUUID from its string representation

        static RZUUID FromPrettyStrFactory(const RZString& s);

        /* Serializes the uuid to a byte string (16 bytes) */
        RZString bytes() const;
        /* fills a string buffer with UUID string representation (16 bytes) */
        void bytes(RZString& out) const;
        /* Fills the char buffer with the RZUUID string representation (16 bytes) */
        void bytes(char* bytes) const;
        /* Converts the uuid to its string representation (32 bytes pretty string) */
        RZString prettyString() const;
        /* fills a string buffer with UUID string representation (32 bytes pretty string)*/
        void prettyString(RZString& s) const;
        /* fills a char buffer with UUID string representation (32 bytes pretty string)*/
        void prettyString(char* res) const;
        /* Hash function for the UUID */
        sz hash() const;

        RZUUID& operator=(const RZUUID& other);

        friend bool operator==(const RZUUID& lhs, const RZUUID& rhs)
        {
            __m128i x = _mm_load_si128((__m128i*) lhs.m_Data);
            __m128i y = _mm_load_si128((__m128i*) rhs.m_Data);

            __m128i neq = _mm_xor_si128(x, y);
            return _mm_test_all_zeros(neq, neq);
        }
        friend bool operator<(const RZUUID& lhs, const RZUUID& rhs)
        {
            // There are no trivial 128-bits comparisons in SSE/AVX
            // It's faster to compare two uint64_t
            uint64_t* x = (uint64_t*) lhs.m_Data;
            uint64_t* y = (uint64_t*) rhs.m_Data;
            return *x < *y || (*x == *y && *(x + 1) < *(y + 1));
        }
        friend bool operator!=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs == rhs); }
        friend bool operator>(const RZUUID& lhs, const RZUUID& rhs) { return rhs < lhs; }
        friend bool operator<=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs > rhs); }
        friend bool operator>=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs < rhs); }

        friend std::ostream& operator<<(std::ostream& stream, const RZUUID& uuid)
        {
            return stream << std::string(uuid.prettyString().c_str());
        }
        friend std::istream& operator>>(std::istream& stream, RZUUID& uuid)
        {
            std::string s;
            stream >> s;
            uuid = FromStrFactory(s.c_str());
            return stream;
        }

    private:
        u8 m_Data[16]; /* The 128-bit byte representation of the RZUUID */

    private:
        static RZUUID FromStrFactory(cstr raw);
        static RZUUID FromStrFactory(const RZString& s);
        static void inline m128iToString(__m128i x, char* mem);
        static __m128i inline stringTom128i(cstr mem);
        static const std::array<u8, 16> prettyStringToBytes(const RZString& prettyStr);
    };
}    // namespace Razix

namespace std {
    template<>
    struct hash<Razix::RZUUID>
    {
        sz operator()(const Razix::RZUUID& uuid) const
        {
            return uuid.hash();
        }
    };
}    // namespace std
