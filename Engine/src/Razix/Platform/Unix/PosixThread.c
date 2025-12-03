#include "Razix/Core/std/thread.h"

#ifdef RAZIX_PLATFORM_UNIX

    #include "Razix/Core/Memory/RZMemoryFunctions.h"
    #include "Razix/Core/std/sprintf.h"

    #include <pthread.h>
    #include <sched.h>           // cput_set_t
    #include <sys/resource.h>    // PRIO_
    #include <time.h>            // for timespec

//---------------------------------------------------------------------------

static RAZIX_TLS char     tls_thread_name[64] = "unknown";
static RAZIX_TLS uint64_t tls_thread_id       = -1;
static RAZIX_TLS int      tls_is_main         = 0;
static RAZIX_TLS int      tls_is_valid        = false;

//---------------------------------------------------------------------------

static void __rz_posix_set_thread_name__(const char* pName)
{
    if (pName || pName[0]) return;

    #ifdef __APPLE__
    int err = pthread_setname_np(pName);
    #else
    int err = pthread_setname_np(pthread_self(), pName);
    #endif
    if (err != 0) {
        // TODO: RAZIX_CORE_ASSERT, once it's ported to C
    }
}

static void _rz_set_thread_priority_low(void)
{
    setpriority(PRIO_PROCESS, 0, 10);    // RZ_THREAD_PRIORITY_LOW
}

static void _rz_set_thread_priority_normal(void)
{
    setpriority(PRIO_PROCESS, 0, 0);    // RZ_THREAD_PRIORITY_NORMAL
}

static void _rz_set_thread_priority_high(void)
{
    setpriority(PRIO_PROCESS, 0, -10);    // RZ_THREAD_PRIORITY_HIGH
}

// once the thread is created and dispatches it makes it easy to set the name and affinity from within the thread this way
static void* _rz_thread_entry(void* args)
{
    RZThreadBootstrap* boot  = (RZThreadBootstrap*) args;
    RZThreadBootstrap  local = *boot;    // keep a local copy for safety instead of using pointer
    rz_free(boot);

    rz_sprintf(tls_thread_name, "[Razix Thread] %s", local.pName);
    tls_thread_id = rz_thread_get_current_id();
    tls_is_valid = true;

    __rz_posix_set_thread_name__(local.pName);
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
    return NULL;
}

//---------------------------------------------------------------------------

RZThreadHandle rz_thread_create(const char* name, RZThreadPriority priority, RZThreadAffinity affinity, RZThreadCallback cb, void* pUserData)
{
    RZThreadHandle handle;
    if (!cb || !pUserData)
        return handle;

    RZThreadBootstrap* pBootStrap = rz_malloc(sizeof(RZThreadBootstrap), 16);
    pBootStrap->cb                = cb;
    pBootStrap->pUserData         = pUserData;
    pBootStrap->affinity          = affinity;
    pBootStrap->pName[0]          = '\0';

    if (name && name[0] != '\0')
        rz_sprintf(pBootStrap->pName, "[Razix Thread] %s", name);
    else
        rz_sprintf(pBootStrap->pName, "[Razix Thread] %s", "<rz_invalid_thread_name>");

    pthread_attr_t attr;
    int            err = pthread_attr_init(&attr);
    if (err != 0) {
        // TODO: Have wrapper in C for RZLog.h
        // RAZIX_CORE_ASSERT_MSG(false, "pthread_attr_init failed: %d", err);
        rz_free(pBootStrap);
        return handle;
    }

    // Set/Get attributes on thread like sched policy, stack size etc.

    err = pthread_create((pthread_t*) &handle, &attr, _rz_thread_entry, pBootStrap);
    pthread_attr_destroy(&attr);
    if (err != 0) {
        // TODO: Have wrapper in C for RZLog.h
        // RAZIX_CORE_ASSERT_MSG(false, "pthread_attr_init failed: %d", err);
        rz_free(pBootStrap);
        return handle;
    }

    return handle;
}

uint64_t rz_thread_exit(uint64_t ret)
{
    pthread_exit((void*) (uintptr_t) ret);
    return ret;
}

uint64_t rz_thread_wait_for_exit(RZThreadHandle threadId, uint64_t timeout_ms)
{
    #if defined RAZIX_PLATFORM_LINUX
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }
    uint64_t res = pthread_timedjoin_np(threadId, NULL, &ts);
    return (res == 0) ? 0 : (res == ETIMEDOUT) ? 1
                                               : -1;
    #else
    return -1;
    #endif
}

void rz_thread_join(RZThreadHandle handle)
{
    int err = pthread_join((pthread_t) handle, NULL);
    if (err != 0) {
        // TODO: Have wrapper in C for RZLog.h
        // RAZIX_CORE_ASSERT_MSG(false, "", err);
    }
}

void rz_thread_detach(RZThreadHandle handle)
{
    int err = pthread_detach((pthread_t) handle);
    if (err != 0) {
        // TODO: Have wrapper in C for RZLog.h
        // RAZIX_CORE_ASSERT_MSG(false, "", err);
    }
}

void rz_thread_set_name(const char* pName)
{
    if (pName)
        __rz_posix_set_thread_name__(pName);
    else
        __rz_posix_set_thread_name__("<rz_invalid_thread_name>");
    rz_sprintf(tls_thread_name, "[Razix Thread] %s", pName ? pName : "<rz_invalid_thread_name>");
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
    // https://man7.org/linux/man-pages/man2/nanosleep.2.html
    if (milliseconds == 0) return;

    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000u;
    ts.tv_nsec = (long) ((milliseconds % 1000u) * 1000000u);

    while (nanosleep(&ts, &ts) == -1) {}
}

void rz_thread_sleep_micor(uint32_t microseconds)
{
    // https://man7.org/linux/man-pages/man2/nanosleep.2.html
    if (microseconds == 0) return;

    struct timespec ts;
    ts.tv_sec  = microseconds / 1000000u;
    ts.tv_nsec = (long) ((microseconds % 1000000u) * 1000u);

    while (nanosleep(&ts, &ts) == -1) {}
}

uint64_t rz_thread_get_current_id(void)
{
    #ifdef __APPLE__
    uint64_t id;
    pthread_threadid_np(NULL, &id);
    return id;
    #else
    return (uint64_t) pthread_self();
    #endif
}

RZThreadHandle rz_thread_get_current_handle(void)
{
    return (RZThreadHandle) pthread_self();
}

void rz_thread_busy_wait_micro(uint32_t microseconds)
{
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    uint64_t target = (uint64_t) start.tv_sec * 1000000ULL +
                      (uint64_t) start.tv_nsec / 1000ULL + microseconds;

    for (;;) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        uint64_t current = (uint64_t) now.tv_sec * 1000000ULL +
                           (uint64_t) now.tv_nsec / 1000ULL;
        if (current >= target + 1)
            break;
    }
}

void rz_thread_set_main(void)
{
    tls_is_main  = 1;
    tls_is_valid = 1;
}

bool rz_thread_is_main(void)
{
    return tls_is_main != 0;
}

const char* rz_thread_get_current_name(void)
{
    return tls_thread_name;
}

int rz_thread_get_priority(void)
{
    int                policy;
    struct sched_param sp;
    int                err = pthread_getschedparam(pthread_self(), &policy, &sp);
    if (err != 0) {
        return -1;
    }
    return sp.sched_priority;
}

int rz_thread_get_affinity(RZThreadHandle handle)
{
    RAZIX_UNUSED(handle);
    return 0;
}

#endif    // RAZIX_PLATFORM_UNIX
