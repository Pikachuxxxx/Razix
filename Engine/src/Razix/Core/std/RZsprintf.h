#ifndef _RZ_SPRINTF_H_
#define _RZ_SPRINTF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "Razix/Core/RZCore.h"

#include <stdarg.h>
#include <stddef.h>    // for size_t used in rz_snprintf / rz_vsnprintf

    // Tasks:
    // [x] Define header API: sprintf, snprintf, vsprintf, "vsnprintf" (Core API, rest all are wrappers over this)
    // - vsnprintf, takes in size_t, for sprintf and vsprintf, we pass UNIT32_MAX to read until \0
    // [x] Define loop
    //   [x] read every % in while and process it, instead of char by char using strchr
    //   [x] Handle cases like no % and other cases etc.
    //   [ ] {Optional} SIMD dot product like optimization instead of strchr
    //   [x] **Define format specifier grammar and DS and enums etc.**
    //   [x] Extract grammar info right after %
    // [x] convert args from int/float etc. to string, cover all specifiers and modifiers (int, unsigned int, float, double, float hex, exponential etc.)
    // [x] Add padding and return the string based on args len
    // [x] Generate Tests covering the grammar using ChatGPT and run them!
    // [x] Test and Fix bugs and edge cases

    RAZIX_API int rz_sprintf(char* buf, const char* fmt, ...);
    RAZIX_API int rz_snprintf(char* buf, size_t size, const char* fmt, ...);
    RAZIX_API int rz_vsprintf(char* buf, const char* fmt, va_list args);
    RAZIX_API int rz_vsnprintf(char* buf, size_t size, const char* fmt, va_list args);

#ifdef __cplusplus
}
#endif

#endif    // _RZ_SPRINTF_H_