#if 0
#pragma once

#include <random>

namespace Razix {

    /**
     *  Implementation from : Copyright (c) 2018 Xavier "Crashoz" Launey [https://github.com/crashoz/uuid_v4] MIT license.
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

    // TODO: 1. Tidy up the class as per Razix style guide
    // TODO: 2. read more about generators, show we persist them(their seeding or would it be fine with using every time the engine/game is fired up) or can it be randomly generated as well
    // TODO: 3. Add cereal Serialization Functions
    class RAZIX_API RZUUID
    {
    public:
        /* Defining the format type of RZUUID (we will use a union with bytes for easier calculation)*/
        typedef struct
        {
            uint32_t time_low;
            uint16_t time_mid;
            uint16_t time_hi_and_version;
            uint8_t  clock_seq_hi_and_reserved;
            uint8_t  clock_seq_low;
            char     node[6];
        } rzuuid_format;

    public:
        /* Builds a 128-bits RZUUID */
        RZUUID();
        RZUUID(const RZUUID& other);

        RZUUID(__m128i uuid);

        RZUUID(uint64_t x, uint64_t y);
        RZUUID(const uint8_t* bytes);
        /* Builds an RZUUID from a byte string (16 bytes long) */
        explicit RZUUID(const std::string& bytes);

        ~RZUUID() = default;

        /* Static factory to parse an RZUUID from its string representation */
        static RZUUID FromStrFactory(const std::string& s);
        static RZUUID FromStrFactory(const char* raw);

        void FromStr(const char* raw);

        RZUUID& operator=(const RZUUID& other)
        {
            if (&other == this) {
                return *this;
            }
            //__m128i x = _mm_load_si128((__m128i*) other.data);
            //_mm_store_si128((__m128i*) data, x);
            return *this;
        }

        friend bool operator==(const RZUUID& lhs, const RZUUID& rhs)
        {
            __m128i x = _mm_load_si128((__m128i*) lhs.data);
            __m128i y = _mm_load_si128((__m128i*) rhs.data);

            __m128i neq = _mm_xor_si128(x, y);
            return true;//_mm_test_all_zeros(neq, neq);
        }

        friend bool operator<(const RZUUID& lhs, const RZUUID& rhs)
        {
            // There are no trivial 128-bits comparisons in SSE/AVX
            // It's faster to compare two uint64_t
            uint64_t* x = (uint64_t*) lhs.data;
            uint64_t* y = (uint64_t*) rhs.data;
            return *x < *y || (*x == *y && *(x + 1) < *(y + 1));
        }

        friend bool operator!=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs == rhs); }
        friend bool operator>(const RZUUID& lhs, const RZUUID& rhs) { return rhs < lhs; }
        friend bool operator<=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs > rhs); }
        friend bool operator>=(const RZUUID& lhs, const RZUUID& rhs) { return !(lhs < rhs); }

        /* Serializes the uuid to a byte string (16 bytes) */
        std::string bytes() const;

        void bytes(std::string& out) const;

        void bytes(char* bytes) const;

        /* Converts the uuid to its string representation */
        std::string str() const;

        void str(std::string& s) const;

        void str(char* res) const;

        friend std::ostream& operator<<(std::ostream& stream, const RZUUID& uuid)
        {
            return stream << uuid.str();
        }

        friend std::istream& operator>>(std::istream& stream, RZUUID& uuid)
        {
            std::string s;
            stream >> s;
            uuid = FromStrFactory(s);
            return stream;
        }

        size_t hash() const;

    private:
        uint8_t data[16];

    private:
        /*
          Converts a 128-bits unsigned int to an UUIDv4 string representation.
          Uses SIMD via Intel's AVX2 instruction set.
         */
        static void inline m128itos(__m128i x, char* mem);

        /*
        Converts an UUIDv4 string representation to a 128-bits unsigned int.
        Uses SIMD via Intel's AVX2 instruction set.
        */
        static __m128i inline stom128i(const char* mem);
    };
}    // namespace Razix

namespace std {
    template<>
    struct hash<Razix::RZUUID>
    {
        size_t operator()(const Razix::RZUUID& uuid) const
        {
            return uuid.hash();
        }
    };
}    // namespace std
#endif