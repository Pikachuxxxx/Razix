#pragma once

#include "Razix/Core/RZCore.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>
typedef LARGE_INTEGER rz_time_stamp;
#else
    #include <time.h>
typedef struct timespec rz_time_stamp;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    RAZIX_API rz_time_stamp rz_time_now(void);
    RAZIX_API double        rz_get_elapsed_ms(rz_time_stamp start, rz_time_stamp end);
    RAZIX_API double        rz_get_elapsed_s(rz_time_stamp start, rz_time_stamp end);

#ifdef __cplusplus
}
#endif