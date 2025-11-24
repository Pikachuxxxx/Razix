#include "RZTime.h"

#ifdef RAZIX_PLATFORM_WINDOWS

rz_time_stamp rz_time_now(void)
{
    LARGE_INTEGER ts;
    QueryPerformanceCounter(&ts);
    return ts;
}

double rz_get_elapsed_ms(rz_time_stamp start, rz_time_stamp end)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (double) (end.QuadPart - start.QuadPart) * 1000.0 / (double) freq.QuadPart;
}

double rz_get_elapsed_s(rz_time_stamp start, rz_time_stamp end)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (double) (end.QuadPart - start.QuadPart) / (double) freq.QuadPart;
}

#else    // POSIX systems

rz_time_stamp rz_time_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts;
}

double rz_get_elapsed_ms(rz_time_stamp start, rz_time_stamp end)
{
    double start_ms = (double) start.tv_sec * 1000.0 + (double) start.tv_nsec / 1e6;
    double end_ms   = (double) end.tv_sec * 1000.0 + (double) end.tv_nsec / 1e6;
    return end_ms - start_ms;
}

double rz_get_elapsed_s(rz_time_stamp start, rz_time_stamp end)
{
    double start_s = (double) start.tv_sec + (double) start.tv_nsec / 1e9;
    double end_s   = (double) end.tv_sec + (double) end.tv_nsec / 1e9;
    return end_s - start_s;
}

#endif    // namespace Razix
