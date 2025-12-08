#include "Razix/Core/std/thread.h"

#ifdef RAZIX_PLATFORM_WINDOWS

    #include "Razix/Core/Memory/RZMemoryFunctions.h"
    #include "Razix/Core/std/sprintf.h"

    #include <stdlib.h>    // Required for exit()

    #include <windows.h>

//---------------------------------------------------------------------------

static RAZIX_TLS char     tls_thread_name[64] = "unknown";
static RAZIX_TLS uint64_t tls_thread_id       = -1;
static RAZIX_TLS int      tls_is_main         = 0;
static RAZIX_TLS int      tls_is_valid        = false;

//---------------------------------------------------------------------------

void __rz_win32_set_thread_name__(const char* name)
{
    wchar_t wname[64];
    MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, RAZIX_THREAD_NAME_MAX_CHARS);
    SetThreadDescription(GetCurrentThread(), wname);
}

void _rz_set_thread_priority_low(void)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
}

void _rz_set_thread_priority_normal(void)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
}

void _rz_set_thread_priority_high(void)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

// once the thread is created and dispatches it makes it easy to set the name and affinity from within the thread this way
static DWORD WINAPI _rz_thread_entry(LPVOID args)
{
    RZThreadBootstrap* boot  = (RZThreadBootstrap*) args;
    RZThreadBootstrap  local = *boot;    // keep a local copy for safety instead of using pointer
    rz_free(boot);

    rz_sprintf(tls_thread_name, "[Razix Thread] %s", local.pName);
    tls_thread_id = rz_thread_get_current_id();
    tls_is_valid  = true;

    __rz_win32_set_thread_name__(local.pName);
    // Note: affinity is ignored on non console platforms, since the processors and OS can result in weirdness
    switch (local.priority) {
        default:
        case RZ_THREAD_PRIORITY_NORMAL:
            _rz_set_thread_priority_normal();
            break;
        case RZ_THREAD_PRIORITY_LOW:
            _rz_set_thread_priority_low();
            break;
        case RZ_THREAD_PRIORITY_HIGH:
            _rz_set_thread_priority_high();
            break;
    }

    if (local.cb)
        local.cb(local.pUserData);

    return 0;
}

//---------------------------------------------------------------------------

RZThreadHandle rz_thread_create(const char* name, RZThreadPriority priority, RZThreadAffinity affinity, RZThreadCallback cb, void* pUserData)
{
    RAZIX_UNUSED(name);
    RAZIX_UNUSED(priority);
    RAZIX_UNUSED(affinity);
    RAZIX_UNUSED(cb);
    RAZIX_UNUSED(pUserData);

    RZThreadHandle handle = 0;
    if (!cb || !pUserData)
        return handle;

    RZThreadBootstrap* pBootStrap = rz_malloc(sizeof(RZThreadBootstrap), 16);
    pBootStrap->cb                = cb;
    pBootStrap->pUserData         = pUserData;
    pBootStrap->affinity          = affinity;
    pBootStrap->pName[0]          = '\0';

    if (name && name[0] != '\0') {
        rz_snprintf(pBootStrap->pName, RAZIX_THREAD_NAME_MAX_CHARS, "[Razix Thread] %s", name);
    } else
        rz_snprintf(pBootStrap->pName, RAZIX_THREAD_NAME_MAX_CHARS, "[Razix Thread] %s", "<rz_invalid_thread_name>");

    HANDLE threadHandle = CreateThread(
        NULL,                // security
        0,                   // stack size
        _rz_thread_entry,    // thread function
        pBootStrap,          // user data
        0,                   // flags
        NULL                 // out thread id store in the TLS variable
    );

    if (!threadHandle) {
        rz_free(pBootStrap);
        return 0;
    }

    tls_thread_id = rz_thread_get_current_id();

    handle = (RZThreadHandle) (uintptr_t) threadHandle;

    return handle;
}

uint64_t rz_thread_exit(uint64_t ret)
{
    ExitThread((DWORD) (uintptr_t) ret);
    return ret;
}

uint64_t rz_thread_wait_for_exit(RZThreadHandle threadId, uint64_t timeout_ms)
{
    WaitForSingleObject((HANDLE) threadId, (DWORD) timeout_ms);
    return timeout_ms;
}

void rz_thread_join(RZThreadHandle thread)
{
    WaitForSingleObject((HANDLE) thread, INFINITE);
    CloseHandle((HANDLE) thread);
}

void rz_thread_detach(RZThreadHandle thread)
{
    CloseHandle((HANDLE) thread);
}

void rz_thread_set_name(const char* pName)
{
    if (pName)
        __rz_win32_set_thread_name__(pName);
    else
        __rz_win32_set_thread_name__("<rz_invalid_thread_name>");
    rz_snprintf(tls_thread_name, RAZIX_THREAD_NAME_MAX_CHARS, "[Razix Thread] %s", pName ? pName : "<rz_invalid_thread_name>");
}

void rz_thread_set_affinity(RZThreadAffinity affinity)
{
    RAZIX_UNUSED(affinity);
    // Note: affinity is ignored on non console platforms, since the processors and OS can result in weirdness
}

void rz_thread_yield(void)
{
    RAZIX_YIELD();
}

void rz_thread_sleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}

void rz_thread_sleep_micro(uint32_t microseconds)
{
    Sleep(microseconds / 1000);
}

void rz_thread_busy_wait_micro(uint32_t microseconds)
{
    LARGE_INTEGER frequency, start, current;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    long long int ticks = (frequency.QuadPart * microseconds) / 1000000;

    do {
        QueryPerformanceCounter(&current);
    } while ((current.QuadPart - start.QuadPart) < ticks);
}

uint64_t rz_thread_get_current_id(void)
{
    return GetCurrentThreadId();
}

RZThreadHandle rz_thread_get_current_handle(void)
{
    return (RZThreadHandle) (uintptr_t) GetCurrentThread();
}

void rz_thread_set_main(void)
{
    tls_is_main   = 1;
    tls_thread_id = rz_thread_get_current_id();
    tls_is_valid  = true;
}

bool rz_thread_is_main(void)
{
    return tls_is_main != 0;
}

const char* rz_thread_get_current_name()
{
    return tls_thread_name;
}

int rz_thread_get_priority(void)
{
    int priority = GetThreadPriority(GetCurrentThread());
    switch (priority) {
        case THREAD_PRIORITY_BELOW_NORMAL:
            return RZ_THREAD_PRIORITY_LOW;
        case THREAD_PRIORITY_NORMAL:
            return RZ_THREAD_PRIORITY_NORMAL;
        case THREAD_PRIORITY_HIGHEST:
            return RZ_THREAD_PRIORITY_HIGH;
        default:
            return RZ_THREAD_PRIORITY_NORMAL;
    }
}

int rz_thread_get_affinity(RZThreadHandle handle)
{
    RAZIX_UNUSED(handle);
    return -1;
}

#endif    // RAZIX_PLATFORM_WINDOWS