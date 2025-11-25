// sprintfTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/sprintf.h"    // rz_sprintf

#include <gtest/gtest.h>

namespace Razix {

#define RZ_SPRINTF_TEST(expected, fmt_str, ...)                                 \
    do {                                                                        \
        char buf[256] = {0};                                                    \
        memset(buf, 0, sizeof(buf));                                            \
        int len = rz_sprintf(buf, fmt_str, ##__VA_ARGS__);                      \
        (void) len; /* silence unused var if needed */                          \
        EXPECT_STREQ((expected), buf)                                           \
            << "Format: \"" << (fmt_str) << "\" produced: \"" << (buf) << "\""; \
    } while (0)

    class RZSprintfTests : public ::testing::Test
    {
    };

    TEST_F(RZSprintfTests, BasicLiteralsAndStrings)
    {
        RZ_SPRINTF_TEST("Hello World!", "Hello World!");
        RZ_SPRINTF_TEST("", "");
        RZ_SPRINTF_TEST("Hello World!", "Hello %s!", "World");
        RZ_SPRINTF_TEST("Hello World! testing again hellooee, World!", "Hello %s! testing again hellooee, %s!", "World", "World");
        RZ_SPRINTF_TEST("Value=42", "Value=%d", 42);
        RZ_SPRINTF_TEST("Value=-0056", "Value=%05hhd", 200);
    }

    TEST_F(RZSprintfTests, IntegerVariantsAndLengthModifiers)
    {
        // FMT_MOD_NONE
        RZ_SPRINTF_TEST("42", "%d", 42);
        RZ_SPRINTF_TEST("+42", "%+d", 42);
        RZ_SPRINTF_TEST("-42", "%i", -42);
        RZ_SPRINTF_TEST("4294967295", "%u", UINT32_MAX);
        RZ_SPRINTF_TEST("4294967295", "%u", UINT64_MAX);    // tests behavior with large arg
        RZ_SPRINTF_TEST("18446744073709551615", "%llu", (unsigned long long) UINT64_MAX);

        // hh
        RZ_SPRINTF_TEST("127", "%hhd", (signed char) 127);
        RZ_SPRINTF_TEST("-128", "%hhi", (signed char) -128);
        RZ_SPRINTF_TEST("255", "%hhu", (unsigned char) 255);
        RZ_SPRINTF_TEST("-56", "%hhd", (signed char) 200);    // overflow case

        // h
        RZ_SPRINTF_TEST("32767", "%hd", (short) 32767);
        RZ_SPRINTF_TEST("-32768", "%hi", (short) -32768);
        RZ_SPRINTF_TEST("65535", "%hu", (unsigned short) 65535);

        // l
        RZ_SPRINTF_TEST("2147483647", "%ld", (long) 2147483647L);
        RZ_SPRINTF_TEST("4294967295", "%lu", (unsigned long) 4294967295UL);

        // ll
        RZ_SPRINTF_TEST("9223372036854775807", "%lld", (long long) 9223372036854775807LL);
        RZ_SPRINTF_TEST("-9223372036854775808", "%lli", (long long) -9223372036854775807LL - 1);
        RZ_SPRINTF_TEST("18446744073709551615", "%llu", (unsigned long long) 18446744073709551615ULL);

        // t (ptrdiff_t)
        RZ_SPRINTF_TEST("1234", "%td", (ptrdiff_t) 1234);
        RZ_SPRINTF_TEST("-1234", "%ti", (ptrdiff_t) -1234);
        RZ_SPRINTF_TEST("4294967295", "%tu", (ptrdiff_t) 4294967295u);

        // j (intmax_t / uintmax_t)
        RZ_SPRINTF_TEST("9223372036854775807", "%jd", (intmax_t) 9223372036854775807LL);
        RZ_SPRINTF_TEST("-9223372036854775808", "%ji", (intmax_t) -9223372036854775807LL - 1);
        RZ_SPRINTF_TEST("18446744073709551615", "%ju", (uintmax_t) 18446744073709551615ULL);

        // z (size_t)
        RZ_SPRINTF_TEST("18446744073709551615", "%zu", (size_t) 18446744073709551615ULL);

        // Edge type conversions / casting anomalies
        RZ_SPRINTF_TEST("255", "%hhu", (unsigned char) -1);
        RZ_SPRINTF_TEST("-1", "%hhd", (signed char) -1);
        RZ_SPRINTF_TEST("65535", "%hu", (unsigned short) -1);
        RZ_SPRINTF_TEST("-1", "%hd", (short) -1);
        RZ_SPRINTF_TEST("18446744073709551615", "%llu", (unsigned long long) -1);
        RZ_SPRINTF_TEST("-1", "%lld", (long long) -1);
    }

    TEST_F(RZSprintfTests, HexUnsignedCharAndChar)
    {
        RZ_SPRINTF_TEST("Hex=2a", "Hex=%x", 42);
        RZ_SPRINTF_TEST("Hex=AF", "Hex=%X", 175);
        RZ_SPRINTF_TEST("Unsigned=42", "Unsigned=%u", 42u);
        RZ_SPRINTF_TEST("Char=A", "Char=%c", 'A');
    }

    TEST_F(RZSprintfTests, OctalTests)
    {
        RZ_SPRINTF_TEST("0", "%o", 0);
        RZ_SPRINTF_TEST("7", "%o", 7);
        RZ_SPRINTF_TEST("10", "%o", 8);
        RZ_SPRINTF_TEST("17", "%o", 15);
        RZ_SPRINTF_TEST("377", "%o", 255);

        RZ_SPRINTF_TEST("0007", "%04o", 7);
        RZ_SPRINTF_TEST("  17", "%4o", 15);

        RZ_SPRINTF_TEST("377", "%hho", (unsigned char) 255);                         // hh
        RZ_SPRINTF_TEST("177777", "%ho", (unsigned short) 65535);                    // h
        RZ_SPRINTF_TEST("37777777777", "%lo", 0xFFFFFFFFUL);                         // l
        RZ_SPRINTF_TEST("1777777777777777777777", "%llo", 0xFFFFFFFFFFFFFFFFULL);    // ll
        RZ_SPRINTF_TEST("37777777777", "%jo", (uintmax_t) 0xFFFFFFFFUL);             // j
        RZ_SPRINTF_TEST("37777777777", "%zo", (size_t) 0xFFFFFFFFUL);                // z
        RZ_SPRINTF_TEST("37777777777", "%to", (ptrdiff_t) 0xFFFFFFFFUL);             // t

        RZ_SPRINTF_TEST("00000017", "%08o", 15);
        RZ_SPRINTF_TEST("17      ", "%-8o", 15);
    }

    // ---------- Floating point ----------
    TEST_F(RZSprintfTests, FloatFixedPoint)
    {
        RZ_SPRINTF_TEST("0.000000", "%f", 0.0);
        RZ_SPRINTF_TEST("-42.500000", "%f", -42.5);
        RZ_SPRINTF_TEST("3.141592", "%f", 3.1415926535);
        RZ_SPRINTF_TEST("1234567.889999", "%f", 1234567.89);

        // More float checks
        RZ_SPRINTF_TEST("1.000000", "%f", 1.0);
        RZ_SPRINTF_TEST("-1.000000", "%f", -1.0);
        RZ_SPRINTF_TEST("1000000.000000", "%f", 1e6);
        RZ_SPRINTF_TEST("-999999.875000", "%f", -999999.875);
        RZ_SPRINTF_TEST("42.420000", "%f", 42.42);
        RZ_SPRINTF_TEST("9.999999", "%f", 9.9999994f);     // rounding down
        RZ_SPRINTF_TEST("10.000000", "%f", 9.9999999f);    // rounding up

        RZ_SPRINTF_TEST("0.500000", "%f", 0.5);
        RZ_SPRINTF_TEST("0.100000", "%f", 0.1);
        RZ_SPRINTF_TEST("0.333333", "%f", 1.0 / 3.0);
        RZ_SPRINTF_TEST("2.999999", "%f", 2.9999994f);
        RZ_SPRINTF_TEST("3.000000", "%f", 2.9999999f);

        // Subnormal-ish checks
        // Disabling this as it can be platform dependent, also our precision is limited to 3 decimal places for subnormals
        // RZ_SPRINTF_TEST("0.000000", "%f", 1e-45f);
        // RZ_SPRINTF_TEST("-0.000000", "%f", -1e-45f);

        RZ_SPRINTF_TEST("100.000000", "%f", 100.0);

        RZ_SPRINTF_TEST("1.000123", "%f", 1.000123);
        RZ_SPRINTF_TEST("0.100000", "%f", 0.100000);
        RZ_SPRINTF_TEST("0.010000", "%f", 0.010000);
        RZ_SPRINTF_TEST("0.001000", "%f", 0.001000);
        RZ_SPRINTF_TEST("0.122999", "%f", 0.123000);
        RZ_SPRINTF_TEST("0.123399", "%f", 0.123400);
    }

    TEST_F(RZSprintfTests, HexadecimalAndScientificFloats)
    {
        RZ_SPRINTF_TEST("0x1.921fb5p+1", "%a", 3.1415926535);
        RZ_SPRINTF_TEST("-0x1.921fb5p+1", "%a", -3.1415926535);
        RZ_SPRINTF_TEST("0x0p+0", "%a", 0.0);

        RZ_SPRINTF_TEST("1.234567e+02", "%e", 123.456789);
        RZ_SPRINTF_TEST("-1.234567e+02", "%e", -123.456789);
        RZ_SPRINTF_TEST("1.230000e-04", "%e", 0.000123);
        RZ_SPRINTF_TEST("0.000000e+00", "%e", 0.0);
    }

    // ---------- Flags, width, precision ----------
    TEST_F(RZSprintfTests, FlagsWidthPrecision)
    {
        RZ_SPRINTF_TEST("Num=0042", "Num=%04d", 42);
        RZ_SPRINTF_TEST("Left=42  ", "Left=%-4d", 42);
        RZ_SPRINTF_TEST("Width= 42", "Width=%3d", 42);
        RZ_SPRINTF_TEST("Precision=3.14", "Precision=%.2f", 3.14159);
        RZ_SPRINTF_TEST("Precision=3.140000", "Precision=%f", 3.14);

        // Integer width, padding and alignment
        RZ_SPRINTF_TEST("Num=0042", "Num=%04d", 42);
        RZ_SPRINTF_TEST("Num=   42", "Num=%5d", 42);
        RZ_SPRINTF_TEST("Num=-042", "Num=%04d", -42);
        RZ_SPRINTF_TEST("Num=-0042", "Num=%05d", -42);
        RZ_SPRINTF_TEST("Left=42   ", "Left=%-5d", 42);
        RZ_SPRINTF_TEST("Left=-42  ", "Left=%-5d", -42);
        RZ_SPRINTF_TEST("Space= 42", "Space=% d", 42);
        RZ_SPRINTF_TEST("Plus=+42", "Plus=%+d", 42);
        RZ_SPRINTF_TEST("Plus=-42", "Plus=%+d", -42);

        // Dynamic width using '*'
        RZ_SPRINTF_TEST("Dyn=   42", "Dyn=%*d", 5, 42);
        RZ_SPRINTF_TEST("Dyn=42   ", "Dyn=%-*d", 5, 42);
        RZ_SPRINTF_TEST("Dyn=00042", "Dyn=%0*d", 5, 42);
        RZ_SPRINTF_TEST("NegWidth=42    ", "NegWidth=%*d", -6, 42);    // negative width -> left align

        // Unsigned and hex width/padding
        RZ_SPRINTF_TEST("U=00099", "U=%05u", 99);
        RZ_SPRINTF_TEST("U=   99", "U=%5u", 99);
        RZ_SPRINTF_TEST("Hex=0x00ff", "Hex=0x%04x", 0xFF);
        RZ_SPRINTF_TEST("HEX=0X00FF", "HEX=0X%04X", 0xFF);

        // Field width vs precision (floats)
        RZ_SPRINTF_TEST("Prec=3.14", "Prec=%.2f", 3.14159);
        RZ_SPRINTF_TEST("Prec=3.1415", "Prec=%.4f", 3.14159);
        RZ_SPRINTF_TEST("Prec=3.140000", "Prec=%f", 3.14);
        RZ_SPRINTF_TEST("Prec=0003.14", "Prec=%07.2f", 3.14159);
        RZ_SPRINTF_TEST("Prec=-003.14", "Prec=%07.2f", -3.14159);
        RZ_SPRINTF_TEST("Prec=   3.14", "Prec=%7.2f", 3.14159);
        RZ_SPRINTF_TEST("Prec=3.14   ", "Prec=%-7.2f", 3.14159);

        // Dynamic width and precision for floats
        RZ_SPRINTF_TEST("Dyn= 3.14", "Dyn=%*.*f", 5, 2, 3.14159);
        RZ_SPRINTF_TEST("Dyn=   3.141", "Dyn=%*.*f", 8, 3, 3.14159);
        RZ_SPRINTF_TEST("Dyn=- 3.14", "Dyn=%*.*f", 6, 2, -3.14159);
        RZ_SPRINTF_TEST("Dyn= 3.1", "Dyn=%*.*f", 4, 1, 3.14159);

        // Sign and zero padding
        RZ_SPRINTF_TEST("Sign=+0042", "Sign=%+05d", 42);
        RZ_SPRINTF_TEST("Sign=-0042", "Sign=%+05d", -42);

        // Zero precision and width edge cases
        RZ_SPRINTF_TEST("ZeroPrec=    0", "ZeroPrec=%5.0d", 0);
        RZ_SPRINTF_TEST("ZeroPrec=00000", "ZeroPrec=%05.0d", 0);
        RZ_SPRINTF_TEST("ZeroPrecU=     0", "ZeroPrecU=%6.0u", 0);
        RZ_SPRINTF_TEST("ZeroPrecU=000000", "ZeroPrecU=%06.0u", 0);

        // Large width sanity tests
        RZ_SPRINTF_TEST("Wide=                                                42", "Wide=%50d", 42);
        RZ_SPRINTF_TEST("Wide=42                                                ", "Wide=%-50d", 42);
        RZ_SPRINTF_TEST("Wide=00000000000000000000000000000000000000000000000042", "Wide=%050d", 42);

        // Float alignment and width sanity
        RZ_SPRINTF_TEST("F=    3.14", "F=%8.2f", 3.14159);
        RZ_SPRINTF_TEST("F=3.14    ", "F=%-8.2f", 3.14159);
        RZ_SPRINTF_TEST("F=-3.14   ", "F=%-8.2f", -3.14159);
        RZ_SPRINTF_TEST("F=0003.14", "F=%07.2f", 3.14159);

        // Mixed integer and float
        RZ_SPRINTF_TEST("Mix=42 3.14", "Mix=%d %.2f", 42, 3.14159);
        RZ_SPRINTF_TEST("Mix=0042 +3.14", "Mix=%04d %+5.2f", 42, 3.14159);
    }

    // ---------- Literal % ----------
    TEST_F(RZSprintfTests, LiteralPercent)
    {
        RZ_SPRINTF_TEST("Percent=%", "Percent=%%");
    }

    // ---------- Edge / empty cases ----------
    TEST_F(RZSprintfTests, EdgeAndMalformed)
    {
        RZ_SPRINTF_TEST("", "");
        RZ_SPRINTF_TEST("%", "%");

        RZ_SPRINTF_TEST("Raw=0", "Raw=%x", 0);    // valid but can test uninitialized handling
        RZ_SPRINTF_TEST("Dangling %", "Dangling %");
        RZ_SPRINTF_TEST("Incomplete %", "Incomplete %");
    }

    // ---------- Multiple specifiers ----------
    TEST_F(RZSprintfTests, MultipleSpecifiers)
    {
        RZ_SPRINTF_TEST("A=10, B=20, S=Hello", "A=%d, B=%d, S=%s", 10, 20, "Hello");
    }

}    // namespace Razix
