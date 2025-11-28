#ifndef _RZ_THREAD_H_
#define _RZ_THREAD_H_

#include "Razix/Core/RZCore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define RAZIX_INVALID_THREAD_ID     (0ull)
#define RAZIX_THREAD_NAME_MAx_CHARS 16
#define RAZIX_TANU_MAX_THREADS      6

    typedef enum RZThreadPriority
    {
        RZ_THREAD_PRIORITY_LOW    = -1,
        RZ_THREAD_PRIORITY_NORMAL = 0,
        RZ_THREAD_PRIORITY_HIGH   = 1,
    } RZThreadPriority;

    typedef enum RZThreadAffinity
    {
        RZ_THREAD_AFFINITY_TANU_GAME_MAIN,    // "TanuGameMain"
        RZ_THREAD_AFFINITY_RENDER,            // "Render"
        RZ_THREAD_AFFINITY_WORKER,            // "Worker"
        RZ_THREAD_AFFINITY_PHYSICS,           // "Physics"
        RZ_THREAD_AFFINITY_AUDIO,             // "Audio"
        RZ_THREAD_AFFINITY_ASSET_IO,          // "AssetIO"
        RZ_THREAD_AFFINITY_EVERYWHERE,        // "Everywhere"
        RZ_THREAD_AFFINITY_COUNT              // RAZIX_TANU_MAX_THREADS + 1
    } RZThreadAffinity;

    // 8-core, all threads have 0 affinity on windows! on consoles they benefit from explict core layout as it's fixed!
    static const uint32_t g_ThreadAffinityMask_8C[RZ_THREAD_AFFINITY_COUNT] =
        {
            /* MAIN      */ (1u << 0),    // core 0
            /* RENDER    */ (1u << 1),    // core 1
            /* WORKER    */ (1u << 2) | (1u << 3) |
                (1u << 4) | (1u << 5),                // cores 2–5
            /* PHYSICS   */ (1u << 6),                // core 6
            /* AUDIO     */ (1u << 7),                // core 7
            /* ASSET_IO  */ (1u << 2) | (1u << 3),    // share some worker cores
            /* EVERYWHERE*/ 0xFFu                     // cores 0–7
    };

    typedef enum RZThreadNames_Tanu
    {
        RZ_THREAD_NAME_TANU_GAME_MAIN = 0,    // "TanuGameMain"
        RZ_THREAD_NAME_RENDER,                // "Render"
        RZ_THREAD_NAME_WORKER,                // "Worker"
        RZ_THREAD_NAME_PHYSICS,               // "Physics"
        RZ_THREAD_NAME_AUDIO,                 // "Audio"
        RZ_THREAD_NAME_ASSET_IO,              // "AssetIO"
        RZ_THREAD_NAME_EVERYWHERE,            // "Everywhere"
        RZ_THREAD_NAME_COUNT                  // RAZIX_TANU_MAX_THREADS + 1
    } RZThreadNames_Tanu;

    static const char* const g_ThreadNames_Tanu[RAZIX_TANU_MAX_THREADS + 1] =
        {
            "TanuGameMain",    // RZ_THREAD_AFFINITY_TANU_GAME_MAIN
            "Render",          // RZ_THREAD_AFFINITY_RENDER
            "Workers",         // RZ_THREAD_AFFINITY_WORKER (Animation, Scripting etc. Jobs all run here)
            "Physics",         // RZ_THREAD_AFFINITY_PHYSICS
            "Audio",           // RZ_THREAD_AFFINITY_AUDIO
            "AssetIO",         // RZ_THREAD_AFFINITY_ASSET_IO
            "Everywhere"       // RZ_THREAD_AFFINITY_EVERYWHERE
    };

    typedef uint64_t RZThreadHandle;    // HANDLE from Win32 API is just a void* which can fit within a u64
    typedef void (*RZThreadCallback)(void* userData);

    typedef struct RZThreadBootstrap
    {
        RZThreadCallback cb;
        void*            pUserData;
        char             pName[RAZIX_THREAD_NAME_MAx_CHARS];
        RZThreadPriority priority;
        RZThreadAffinity affinity;
    } RZThreadBootstrap;

    RZThreadHandle rz_thread_create(const char* name, RZThreadPriority priority, RZThreadAffinity affinity, RZThreadCallback cb, void* userData);

    uint64_t rz_thread_exit(uint64_t ret);
    uint64_t rz_thread_wait_for_exit(RZThreadHandle threadId, uint64_t timeout_ms);
    void     rz_thread_join(RZThreadHandle thread);
    void     rz_thread_detach(RZThreadHandle thread);
    void     rz_thread_set_name(const char* pName);
    void     rz_thread_set_affinity(RZThreadAffinity affinity);
    void     rz_thread_yield(void);

    void rz_thread_sleep(uint32_t milliseconds);
    void rz_thread_sleep_micro(uint32_t microseconds);

    void rz_thread_busy_wait_micro(uint32_t microseconds);

    uint64_t       rz_thread_get_current_id(void);
    RZThreadHandle rz_thread_get_current_handle(void);

    void rz_thread_set_main(void);
    bool rz_thread_is_main(void);

    const char* rz_thread_get_current_name();

    int rz_thread_get_priority(void);
    int rz_thread_get_affinity(RZThreadHandle handle);

// TODO: Implement pico pause for CPU/GPU/IO and other bubbles in C++ to inline it
// https://software.intel.com/sites/default/files/m/d/4/1/d/8/17689_w_spinlock.pdf
// Profile and see how many cycles you need to spin on it.
#define RZ_THREAD_PICO_PAUSE \
    {                        \
        _mm_pause();         \
    }

#ifdef __cplusplus
}
#endif

#endif    // _RZ_THREAD_H_
