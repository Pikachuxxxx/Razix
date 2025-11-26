#include "sprintf.h"

#include <math.h>    // pow
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>    // for newer std types
#include <string.h>    // strchr, strlen

#define READ_UNTIL_STR_NULL            0xFFFFFFFF
#define FMT_SPECIFIER_DELIMITER_CHR    '%'
#define DEFAULT_FLOATHEX_PRECISION     6
#define DEFAULT_FLOAT_PRECISION        6
#define DEFAULT_DOUBLE_PRECISION       3
#define IEEE754_DOUBLE_MANTISSA_DIGITS 52
#define IEEE754_DOUBLE_EXPONENT_DIGITS 11
#define IEEE754_DOUBLE_SIGN_DIGITS     1
#define IEEE754_DOUBLE_EXPONENT_BIAS   1023LLU    // [2^11 - 2 (0 and inf/nan)] / 2 = 1023
#define IEEE754_FLOAT_MANTISSA_DIGITS  23
#define IEEE754_FLOAT_EXPONENT_DIGITS  8
#define IEEE754_FLOAT_SIGN_DIGITS      1
#define IEEE754_FLOAT_EXPONENT_BIAS    127U    // [2^8 - 2 (0 and inf/nan)] / 2 = 127
#define MAX_INTERMEDIATE_BUFFER_SIZE   256

#define ARG_TABLE_SIGNED(F, base)             \
    F(FMT_MOD_NONE, int, int, base)           \
    F(FMT_MOD_hh, char, int, base)            \
    F(FMT_MOD_h, short, int, base)            \
    F(FMT_MOD_l, long, long, base)            \
    F(FMT_MOD_ll, long long, long long, base) \
    F(FMT_MOD_j, intmax_t, intmax_t, base)    \
    F(FMT_MOD_t, ptrdiff_t, ptrdiff_t, base)  \
    // TODO: Support ssize_t

#define ARG_TABLE_UNSIGNED(F, base)                             \
    F(FMT_MOD_NONE, unsigned int, unsigned int, base)           \
    F(FMT_MOD_hh, unsigned char, unsigned int, base)            \
    F(FMT_MOD_h, unsigned short, unsigned int, base)            \
    F(FMT_MOD_l, unsigned long, unsigned long, base)            \
    F(FMT_MOD_ll, unsigned long long, unsigned long long, base) \
    F(FMT_MOD_j, uintmax_t, uintmax_t, base)                    \
    F(FMT_MOD_t, ptrdiff_t, ptrdiff_t, base)                    \
    F(FMT_MOD_z, size_t, size_t, base)

#define GET_ARG_S(mod, vartype, argtype, base)                                  \
    case mod: {                                                                 \
        vartype v = (vartype) va_arg(args, argtype);                            \
        isNeg     = v < 0;                                                      \
        val       = isNeg ? -v : v;                                             \
        writtenBytes += itoa(val, (buf_span) {num_buf, sizeof(num_buf)}, base); \
        break;                                                                  \
    }

#define GET_ARG_U(mod, vartype, argtype, base)                                \
    case mod: {                                                               \
        vartype v = (vartype) va_arg(args, argtype);                          \
        writtenBytes += itoa(v, (buf_span) {num_buf, sizeof(num_buf)}, base); \
        break;                                                                \
    }

#define GET_ARG_CAPS(mod, vartype, argtype, base)                             \
    case mod: {                                                               \
        vartype v = (vartype) va_arg(args, argtype);                          \
        writtenBytes += itoA(v, (buf_span) {num_buf, sizeof(num_buf)}, base); \
        break;                                                                \
    }

typedef enum fmt_flag
{
    FMT_FLAG_NONE       = 0,
    FMT_FLAG_SIGN       = 1 << 0,
    FMT_FLAG_LEFT_ALIGN = 1 << 1,
    FMT_FLAG_ZERO       = 1 << 2,
    FMT_FLAG_SPACE      = 1 << 3,
    FMT_FLAG_ALT_FORM   = 1 << 4,
} fmt_flag;

typedef enum fmt_mod
{
    FMT_MOD_NONE,
    FMT_MOD_l,     // long
    FMT_MOD_ll,    // long long
    FMT_MOD_L,     // long double
    FMT_MOD_h,     // short / unsigned short
    FMT_MOD_hh,    // signed char / unsigned char
    FMT_MOD_t,     // ptrdiff_t
    FMT_MOD_j,     // intmax_t/uintmax_t
    FMT_MOD_z,     // size_t
} fmt_mod;

// specifier: d, i, u, o, x, X, f, e, g, a, c, s, p, %
// TODO: support %F, %E, %G
typedef enum fmt_spec
{
    FMT_SPEC_NONE,
    FMT_SPEC_d,             // int
    FMT_SPEC_i,             // int
    FMT_SPEC_u,             // unsigned int
    FMT_SPEC_o,             // octal
    FMT_SPEC_x,             // hexadecimal
    FMT_SPEC_X,             // CAPS hexadecimal
    FMT_SPEC_f,             // floating
    FMT_SPEC_e,             // exponential
    FMT_SPEC_g,             // floating or exponential whichever is the shortest
    FMT_SPEC_a,             // hex floating point
    FMT_SPEC_c,             // char
    FMT_SPEC_s,             // string,
    FMT_SPEC_p,             // pointer,
    FMT_SPEC_PERCENTAGE,    // % print as-is
} fmt_spec;

static const fmt_flag flag_map[128] = {
    ['+'] = FMT_FLAG_SIGN,
    ['-'] = FMT_FLAG_LEFT_ALIGN,
    ['0'] = FMT_FLAG_ZERO,
    [' '] = FMT_FLAG_SPACE,
    ['#'] = FMT_FLAG_ALT_FORM,
};

static const fmt_mod mod_map[128] = {
    ['l'] = FMT_MOD_l,
    ['L'] = FMT_MOD_L,
    ['h'] = FMT_MOD_h,
    ['t'] = FMT_MOD_t,
    ['j'] = FMT_MOD_j,
    ['z'] = FMT_MOD_z,
};

static const fmt_spec spec_map[128] = {
    ['d'] = FMT_SPEC_d,
    ['i'] = FMT_SPEC_i,
    ['u'] = FMT_SPEC_u,
    ['o'] = FMT_SPEC_o,
    ['x'] = FMT_SPEC_x,
    ['X'] = FMT_SPEC_X,
    ['f'] = FMT_SPEC_f,
    ['e'] = FMT_SPEC_e,
    ['g'] = FMT_SPEC_g,
    ['a'] = FMT_SPEC_a,
    ['c'] = FMT_SPEC_c,
    ['s'] = FMT_SPEC_s,
    ['p'] = FMT_SPEC_p,
    ['%'] = FMT_SPEC_PERCENTAGE,
};

typedef struct buf_span
{
    char*  ptr;
    size_t len;
} buf_span;

static uint64_t _itoa(uint64_t val, buf_span buf, int base, bool caps)
{
    if (buf.ptr == NULL || buf.len == 0)
        return 0;

    if (base < 2 || base > 16)
        return 0;

    uint64_t           bytes      = 0;
    static const char* charset[2] = {"0123456789abcdef", "0123456789ABCDEF"};

    char* const out_begin = buf.ptr;

    do {
        if (bytes + 1 >= buf.len)
            break;

        *buf.ptr++ = charset[caps][val % base];
        val /= base;
        bytes++;
    } while (val);

    {
        size_t i = 0;
        size_t j = (bytes == 0) ? 0 : (size_t) bytes - 1;

        while (i < j) {
            char tmp     = out_begin[i];
            out_begin[i] = out_begin[j];
            out_begin[j] = tmp;
            ++i;
            --j;
        }
    }

    out_begin[bytes] = '\0';

    return bytes;
}

static uint64_t itoA(uint64_t val, buf_span buf, int base)
{
    return _itoa(val, buf, base, true);
}

static uint64_t itoa(uint64_t val, buf_span buf, int base)
{
    return _itoa(val, buf, base, false);
}

static uint64_t ftoa(double val, buf_span buf, uint32_t precision)
{
    // double IEEE-745: [sign:1] [exponent:11] [mantissa:52]
    // bias = 1023, all exponents have bias to store as positive numbers
    // normalized => exponent != 0, subnormal => exponent == 0
    uint64_t bytes = 0;
    union
    {
        double   d;
        uint64_t u;
    } v           = {val};
    uint64_t bits = v.u;

    const uint64_t EXP_MASK  = (1ULL << IEEE754_DOUBLE_EXPONENT_DIGITS) - 1;
    const uint64_t MANT_MASK = (1ULL << IEEE754_DOUBLE_MANTISSA_DIGITS) - 1;

    int64_t  exponent    = (bits >> IEEE754_DOUBLE_MANTISSA_DIGITS) & EXP_MASK;    // remove 52 bits and get the trailing 11 bits
    uint64_t mantissa    = bits & MANT_MASK;                                       // get the trailing 52-bits
    bool     is_denormal = false;

    if (exponent == 0) {
        if (mantissa == 0) {
            *buf.ptr++ = '0';
            *buf.ptr++ = '.';
            bytes += 2;
            for (uint32_t i = 0; i < precision; i++) {
                *buf.ptr++ = '0';
                bytes++;
            }
            return bytes;
        } else {
            is_denormal = true;
            exponent    = 1LLU - IEEE754_DOUBLE_EXPONENT_BIAS;
        }
    } else if (exponent == 0x7FF) {
        // special cases
        if (mantissa == 0) {
            //  INF
            memcpy(buf.ptr, "inf", 3 * sizeof(char));
        } else {
            // NAN
            memcpy(buf.ptr, "nan", 3 * sizeof(char));
        }
        bytes += 3;
        return bytes;
    } else {
        // Normalized
        exponent = exponent - IEEE754_DOUBLE_EXPONENT_BIAS;    // unbiased
    }

    uint64_t int_part  = 0;
    uint64_t frac_part = 0;
    uint64_t shift     = 0;
    if (exponent < 0) {
        // All bits are fractional
        int_part = 0;
        shift    = IEEE754_DOUBLE_MANTISSA_DIGITS + (-exponent);
        if (is_denormal) {
            frac_part = mantissa;
        } else {
            frac_part = (1ULL << 52) | mantissa;
        }
    } else if (exponent >= 52) {
        // All bits contribute to integer part (1 + mantissa)
        int_part  = (1ULL << exponent) | (mantissa << (exponent - IEEE754_DOUBLE_MANTISSA_DIGITS));
        frac_part = 0;
    } else {
        // Split between integer and fraction
        shift = 52 - exponent;
        // get top int bits (1 + mantissa)
        int_part = (1ULL << exponent) | (mantissa >> shift);
        // get lower fract part
        frac_part = mantissa & ((1ULL << shift) - 1);
    }

    bytes += itoa(int_part, buf, 10);
    buf.ptr += bytes;
    *buf.ptr++ = '.';
    bytes++;
    // convert fract to string
    if (precision > buf.len - bytes) {
        precision = (uint32_t) (buf.len - bytes);
    }
    for (uint32_t i = 0; i < precision; i++) {
        frac_part *= 10;
        uint64_t digit = frac_part >> shift;
        *buf.ptr++     = "0123456789"[digit % 10];
        bytes++;
        frac_part &= ((1ULL << shift) - 1);
    }

    return bytes;
}

static uint64_t ftohex(double val, buf_span buf)
{
    uint64_t bytes = 0;
    union
    {
        double   d;
        uint64_t u;
    } v           = {val};
    uint64_t bits = v.u;

    const uint64_t EXP_MASK  = (1ULL << IEEE754_DOUBLE_EXPONENT_DIGITS) - 1;
    const uint64_t MANT_MASK = (1ULL << IEEE754_DOUBLE_MANTISSA_DIGITS) - 1;

    // TODO Handle 0x0+p0, nan and inf
    int64_t  exponent        = (bits >> IEEE754_DOUBLE_MANTISSA_DIGITS) & (EXP_MASK);    // remove 52 bits and get the trailing 11 bits
    uint64_t mantissa        = bits & MANT_MASK;                                         // get the trailing 52-bits
    bool     leadingMantissa = false;

    if ((mantissa == 0 && exponent == 0) || val == 0.0) {
        memcpy(buf.ptr, "0x0p+0", 6 * sizeof(char));
        bytes += 6;
        return bytes;
    }

    if (exponent == EXP_MASK) {
        if (mantissa == 0) {
            memcpy(buf.ptr, "inf", 3 * sizeof(char));
        } else {
            memcpy(buf.ptr, "nan", 3 * sizeof(char));
        }
        bytes += 3;
        return bytes;
    }

    if (exponent == 0) {
        exponent = 1LLU - IEEE754_DOUBLE_EXPONENT_BIAS;
    } else {
        leadingMantissa = true;
        exponent        = exponent - IEEE754_DOUBLE_EXPONENT_BIAS;
    }

    *buf.ptr++ = '0';
    *buf.ptr++ = 'x';
    *buf.ptr++ = leadingMantissa ? '1' : '0';
    *buf.ptr++ = '.';
    bytes += 4;
    char hex[16] = {0};
    itoa(mantissa, (buf_span) {hex, sizeof(hex)}, 16);
    memcpy(buf.ptr, hex, DEFAULT_FLOATHEX_PRECISION * sizeof(char));
    bytes += DEFAULT_FLOATHEX_PRECISION;
    buf.ptr += DEFAULT_FLOATHEX_PRECISION;

    *buf.ptr++ = 'p';
    *buf.ptr++ = exponent > 0 ? '+' : '-';
    bytes += 2;
    char exp[4] = {0};
    itoa(exponent, (buf_span) {exp, sizeof(exp)}, 10);
    memcpy(buf.ptr, exp, 4 * sizeof(char));
    bytes += 4;

    return bytes;
}

static uint64_t etoa(double val, buf_span buf, uint32_t precision, bool caps)
{
    uint64_t bytes = 0;

    if (val == 0.0) {
        memcpy(buf.ptr, "0.000000e+00", 12 * sizeof(char));
        bytes += 12;
        return bytes;
    }

    int32_t base10_exponent = 0;
    while (val >= 10.0) {
        val /= 10.0;
        base10_exponent++;
    }
    while (val < 1.0 && val != 0.0) {
        val *= 10.0;
        base10_exponent--;
    }

    // write the fract part as is
    bytes += ftoa(val, buf, precision);
    buf.ptr += bytes;

    *buf.ptr++ = caps ? 'E' : 'e';
    *buf.ptr++ = base10_exponent > 0 ? '+' : '-';
    bytes += 2;
    if (base10_exponent < 10) {
        *buf.ptr++ = '0';
        bytes++;
    }
    char     expv[16] = {0};
    uint64_t write    = itoa(base10_exponent < 0 ? -base10_exponent : base10_exponent, (buf_span) {expv, sizeof(expv)}, 10);
    memcpy(buf.ptr, expv, write * sizeof(char));
    bytes += write;

    return bytes;
}

int rz_sprintf(char* buf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = rz_vsnprintf(buf, READ_UNTIL_STR_NULL, fmt, args);
    va_end(args);
    return ret;
}

int rz_snprintf(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = rz_vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

int rz_vsprintf(char* buf, const char* fmt, va_list args)
{
    return rz_vsnprintf(buf, READ_UNTIL_STR_NULL, fmt, args);
}

int rz_vsnprintf(char* buf, size_t size, const char* fmt, va_list args)
{
    bool bReadUntilNullChar = (size == READ_UNTIL_STR_NULL);

    if (size == 0) return 0;
    if (!buf || !fmt) return -1;

    size_t writtenBytes = 0;

    // get the first %
    // TODO: use SIMD path instead of strchr to load 16 bytes at once into SIMD registers
    char*       scan_elem = strchr(fmt, FMT_SPECIFIER_DELIMITER_CHR);
    const char* format    = fmt;

    if (bReadUntilNullChar)
        size = strlen(fmt);    // TODO: write your own, maybe use SIMD to quick count until \0? or use dotproduct like magic?

    if (size == 0) return 0;

    // not found copy the whole string as-is
    if (!scan_elem) {
        size_t fmt_len = strlen(fmt);
        if (fmt_len >= size)
            fmt_len = size;

        memcpy(buf, fmt, fmt_len);
        buf[fmt_len++] = '\0';
        writtenBytes += fmt_len;
        return (int) writtenBytes;
    }

    while (scan_elem && *scan_elem != '\0') {
        scan_elem = strchr(format, FMT_SPECIFIER_DELIMITER_CHR);

        // copy remainder of fmt
        if (!scan_elem) {
            size_t len = strlen(format);
            memcpy(buf + writtenBytes, format, len);
            writtenBytes += len;
            buf[writtenBytes++] = '\0';
            return (int) writtenBytes;
        }

        // copy buffer until %
        size_t literalLen = scan_elem - format;
        if (writtenBytes + literalLen > size) literalLen = size - writtenBytes;
        memcpy(buf + writtenBytes, format, literalLen);
        writtenBytes += literalLen;

        // move past %
        scan_elem++;

        // Format Specifier Grammar:
        // %[flags][width].[precision][modifier][specifier]
        // Flags: +, -, 0, , #
        // width: [0-9], *
        // precision: [0-9]
        // modifier: z, l, ll, L, h, hh, t, j
        // specifier: d, i, u, o, x, X, f, F, e, E, g, G, a, A, c, s, p, %

        fmt_flag flags = FMT_FLAG_NONE;
        while (flag_map[*(unsigned char*) scan_elem]) {
            flags |= flag_map[*(unsigned char*) scan_elem];
            scan_elem++;
        }

        uint32_t width            = 0;
        bool     bHasDynamicWidth = false;
        if (*scan_elem == '*') bHasDynamicWidth = true;
        if (bHasDynamicWidth) {
            int value = 0;
            value     = va_arg(args, int);
            scan_elem++;
            if (value < 0) {
                value = -value;
                flags = FMT_FLAG_LEFT_ALIGN;
            }
            width = value;
        }

        while (*scan_elem >= '0' && *scan_elem <= '9') {
            width = width * 10 + (*scan_elem - '0');
            scan_elem++;
        }

        bool     bHasPrecision = (*scan_elem == '.');
        uint32_t precision     = DEFAULT_FLOAT_PRECISION;
        if (bHasPrecision) {
            precision = 0;
            scan_elem++;
        }

        bool bHasDynamicPrecision = false;
        if (*scan_elem == '*') bHasDynamicPrecision = true;
        if (bHasDynamicPrecision) {
            int value = 0;
            value     = va_arg(args, int);
            scan_elem++;
            if (value < 0) {
                value = -value;
            }
            precision = value;
        }

        while (bHasPrecision && *(scan_elem) >= '0' && *(scan_elem) <= '9') {
            precision += *(scan_elem) - '0';
            scan_elem++;
        }

        fmt_mod modifier = FMT_MOD_NONE;
        if (mod_map[*(unsigned char*) scan_elem]) {
            modifier = mod_map[*(unsigned char*) scan_elem];
            scan_elem++;

            if (*(scan_elem) == 'h') {
                modifier = FMT_MOD_hh;
                scan_elem++;
            }

            if (*(scan_elem) == 'l') {
                modifier = FMT_MOD_ll;
                scan_elem++;
            }
        }

        // get the specifier and value and calculate it's width before we add any padding
        // padding fmt/total width with precision length: [sign][left padding][value str len][right padding]
        // must be present!
        fmt_spec spec = spec_map[*(unsigned char*) scan_elem];

        // fuck off!
        if (spec == FMT_SPEC_NONE) {
            size_t len = strlen(format + writtenBytes);
            memcpy(buf + writtenBytes, format + writtenBytes, len);
            writtenBytes += len;
            return (int) writtenBytes;
        }

        char pad_char     = (flags & FMT_FLAG_ZERO) && !(flags & FMT_FLAG_LEFT_ALIGN) ? '0' : ' ';
        int  pad_trailing = (flags & FMT_FLAG_LEFT_ALIGN) == FMT_FLAG_LEFT_ALIGN;
        int  pad_leading  = ((flags & FMT_FLAG_ZERO) == FMT_FLAG_ZERO) || (!pad_trailing && width > 0);

        size_t byteBeforeArgs = writtenBytes;

        uint64_t val                                   = (uint64_t) 0x0;
        bool     isNeg                                 = false;
        char     num_buf[MAX_INTERMEDIATE_BUFFER_SIZE] = {0};
        switch (spec) {
            case FMT_SPEC_d:
            case FMT_SPEC_i:
                switch (modifier) {
                    default:
                        ARG_TABLE_SIGNED(GET_ARG_S, 10)
                }
                break;
            case FMT_SPEC_u:
                switch (modifier) {
                    default:
                        ARG_TABLE_UNSIGNED(GET_ARG_U, 10)
                }
                break;
            case FMT_SPEC_s: {
                const char* str = va_arg(args, const char*);
                size_t      len = strlen(str);
                if (len > 256) len = 256;    // cap it
                memcpy(num_buf, str, len);
                writtenBytes += len;
            } break;
            case FMT_SPEC_c: {
                num_buf[0] = (char) va_arg(args, int);
                writtenBytes++;
            } break;
            case FMT_SPEC_x:
                switch (modifier) {
                    default:
                        ARG_TABLE_UNSIGNED(GET_ARG_U, 16)
                }
                break;
            case FMT_SPEC_X:
                switch (modifier) {
                    default:
                        ARG_TABLE_UNSIGNED(GET_ARG_CAPS, 16)
                }
                break;
            case FMT_SPEC_o:
                switch (modifier) {
                    default:
                        ARG_TABLE_UNSIGNED(GET_ARG_U, 8)
                }
                break;
            case FMT_SPEC_f:
                //case FMT_SPEC_F:
                switch (modifier) {
                    default:
                    case FMT_MOD_NONE: {
                        double v = va_arg(args, double);
                        isNeg    = v < 0;
                        writtenBytes += ftoa(isNeg ? -v : v, (buf_span) {num_buf, sizeof(num_buf)}, precision);
                        break;
                    }
                }
                break;
                //case FMT_SPEC_E:
            case FMT_SPEC_e: {
                double v = va_arg(args, double);
                isNeg    = v < 0;
                writtenBytes += etoa(isNeg ? -v : v, (buf_span) {num_buf, sizeof(num_buf)}, precision, spec != FMT_SPEC_e /* TODO: replace with FMT_SPEC_E */);
                break;
            }
            case FMT_SPEC_g:
                //case FMT_SPEC_G:
                //case FMT_SPEC_A:
            case FMT_SPEC_a: {
                double v = va_arg(args, double);
                isNeg    = v < 0;
                writtenBytes += ftohex(isNeg ? -v : v, (buf_span) {num_buf, sizeof(num_buf)});
                break;
            } break;
            case FMT_SPEC_p:    // pointer
            {
                void* ptr = va_arg(args, void*);
#ifdef _MSC_VER
                char     hex[16] = {0};
                uint64_t len     = itoA((uint64_t) ptr, (buf_span) {hex, sizeof(hex)}, 16);
                memset(num_buf, '0', 16 - len);
                memcpy(num_buf + (16 - len), hex, len);
                writtenBytes += 16;
#elif defined(__clang__) || defined(__GCC__)
                num_buf[0] = '0';
                num_buf[1] = 'x';
                writtenBytes += 2;
                writtenBytes += itoa((uint64_t) ptr, (buf_span){num_buf + 2, sizeof(num_buf) - 2}, 16);
#endif
            } break;
            case FMT_SPEC_PERCENTAGE:    // literal %
            {
                num_buf[0] = '%';
                writtenBytes++;
                break;
            }
            default:
                val = (int) va_arg(args, int);
                writtenBytes += itoa(val, (buf_span) {num_buf, sizeof(num_buf)}, 10);
                break;
        }
        scan_elem++;

        if (isNeg || (flags & FMT_FLAG_SIGN)) {
            buf[byteBeforeArgs++] = isNeg ? '-' : '+';
            writtenBytes++;
        }
        // Now add padding
        size_t argsBytesWritten = writtenBytes - byteBeforeArgs;

        size_t padding_needed = width - argsBytesWritten - (isNeg || (flags & FMT_FLAG_SIGN));

        // left padding
        memset(buf + byteBeforeArgs, pad_char, padding_needed * pad_leading);
        if (padding_needed * pad_leading) {
            size_t left_padding = padding_needed * pad_leading;
            byteBeforeArgs += left_padding;
            writtenBytes += left_padding;
        }

        if (flags & FMT_FLAG_SPACE) {
            buf[byteBeforeArgs++] = ' ';
            writtenBytes++;
        }

        // clamp to fit
        memcpy(buf + byteBeforeArgs, num_buf, argsBytesWritten);

        // right padding
        memset(buf + writtenBytes, pad_char, padding_needed * pad_trailing);
        if (padding_needed * pad_trailing)
            writtenBytes += padding_needed * pad_trailing;

        format = scan_elem;
    }

    buf[writtenBytes++] = '\0';

    return (int) writtenBytes;
}
