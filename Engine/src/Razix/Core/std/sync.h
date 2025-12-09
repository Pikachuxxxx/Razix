#ifndef _RZ_SYNC_H_
#define _RZ_SYNC_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#define RAZIX_CS_DEF_SPIN_CNT       100    // TODO: to be measures using __mm_pause or how long threads take to switch
#define RAZIX_OS_MUTEX_STORAGE_SIZE 64 * 2
#define RAZIX_OS_COND_STORAGE_SIZE  64

// Fast PImpl idiom to avoid including heavy OS specific headers in the main codebase
RAZIX_ALIGN_AS(RAZIX_CACHE_LINE_SIZE)
typedef struct RZCriticalSection
{
    u32 m_SpinCount;
    union
    {
        u8    buffer[RAZIX_OS_MUTEX_STORAGE_SIZE];
        void* aligner;
    } m_Internal;
} RZCriticalSection;

RAZIX_ALIGN_AS(RAZIX_CACHE_LINE_SIZE) typedef struct RZConditionalVar 
{
    union 
    {
        u8 buffer[RAZIX_OS_COND_STORAGE_SIZE];
        void* aligner;
    } m_Internal;
} RZConditionalVar;

// Mutex API
RZCriticalSection rz_critical_section_create(void);
RZCriticalSection rz_critical_section_create_ex(u32 spinCount);
void              rz_critical_section_destroy(RZCriticalSection* cs);

void rz_critical_section_lock(RZCriticalSection* cs);
bool rz_critical_section_try_lock(RZCriticalSection* cs);
void rz_critical_section_unlock(RZCriticalSection* cs);

// Conditional Variable API
RZConditionalVar rz_conditional_var_create(void);
void             rz_conditional_var_destroy(RZConditionalVar* cv);

void rz_conditional_var_wait(RZConditionalVar* cv, RZCriticalSection* cs);
void rz_conditional_var_signal(RZConditionalVar* cv);
void rz_conditional_var_broadcast(RZConditionalVar* cv);

#endif    // _RZ_SYNC_H_
