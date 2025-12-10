#ifndef _RZ_SYNC_H_
#define _RZ_SYNC_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#define RAZIX_CS_DEF_SPIN_CNT       100    // TODO: to be measures using __mm_pause or how long threads take to switch
#define RAZIX_OS_MUTEX_STORAGE_SIZE (64 * 2)
#define RAZIX_OS_COND_STORAGE_SIZE  64

// Fast PImpl idiom to avoid including heavy OS specific headers in the main codebase
RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_critical_section
{
    u32 m_SpinCount;
    union
    {
        u8 buffer[RAZIX_OS_MUTEX_STORAGE_SIZE - sizeof(u32)];
        // void* aligner;
    } m_Internal;
} rz_critical_section;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_cond_var
{
    union
    {
        u8    buffer[RAZIX_OS_COND_STORAGE_SIZE];
        void* aligner;
    } m_Internal;
} rz_cond_var;

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    // Mutex API
    RAZIX_API rz_critical_section rz_critical_section_create(void);
    RAZIX_API rz_critical_section rz_critical_section_create_ex(u32 spinCount);
    RAZIX_API void                rz_critical_section_destroy(rz_critical_section* cs);

    RAZIX_API void rz_critical_section_lock(rz_critical_section* cs);
    RAZIX_API bool rz_critical_section_try_lock(rz_critical_section* cs);
    RAZIX_API void rz_critical_section_unlock(rz_critical_section* cs);

    // Conditional Variable API
    RAZIX_API rz_cond_var rz_conditional_var_create(void);
    RAZIX_API void        rz_conditional_var_destroy(rz_cond_var* cv);

    RAZIX_API void rz_conditional_var_wait(rz_cond_var* cv, rz_critical_section* cs);
    RAZIX_API void rz_conditional_var_signal(rz_cond_var* cv);
    RAZIX_API void rz_conditional_var_broadcast(rz_cond_var* cv);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_SYNC_H_
