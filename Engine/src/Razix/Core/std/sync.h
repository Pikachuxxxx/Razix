#ifndef _RZ_SYNC_H_
#define _RZ_SYNC_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>
#elif defined RAZIX_PLATFORM_UNIX
    #include <pthread.h>
#endif

#define RAZIX_CS_DEF_SPIN_CNT 100    // TODO: to be measures using __mm_pause or how long threads take to switch

namespace Razix {

    class RZConditionalVar;

    class alignas(RAZIX_CACHE_LINE_SIZE) RAZIX_API RZCriticalSection
    {
    public:
        RZCriticalSection()  = default;
        ~RZCriticalSection() = default;
        // Mutexes and locking primitves cannot be copied/moved
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZCriticalSection);

        void init(u32 spinCount = RAZIX_CS_DEF_SPIN_CNT);
        void destroy();

        void lock();
        bool try_lock();
        void unlock();

    private:
        u32 m_SpinCount;
#ifdef RAZIX_PLATFORM_WINDOWS
        CRITICAL_SECTION m_CS;
#elif defined RAZIX_PLATFORM_UNIX
        pthread_mutex_t m_CS;
#endif

        friend class RZConditionalVar;
    };
    static_assert(sizeof(RZCriticalSection) <= 2 * RAZIX_CACHE_LINE_SIZE, "RZCriticalSection must be less than 2 cache lines");
    static_assert(alignof(RZCriticalSection) == RAZIX_CACHE_LINE_SIZE, "RZCriticalSection must be cache-line aligned");

    //---------------------------------------------------------------------------

    class alignas(RAZIX_CACHE_LINE_SIZE) RAZIX_API RZConditionalVar
    {
    public:
        RZConditionalVar()  = default;
        ~RZConditionalVar() = default;
        // Mutexes and locking primitves cannot be copied/moved
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZConditionalVar);

        void init();
        void destroy();

        void signal();
        void broadcast();
        void wait(RZCriticalSection* cs);
        void wait(RZCriticalSection* cs, u32 timeout);

    private:
#ifdef RAZIX_PLATFORM_WINDOWS
        CONDITIONLA_VARIABLE m_CV;
#elif defined RAZIX_PLATFORM_UNIX
        pthread_cond_t m_CV;
#endif
    };
    static_assert(sizeof(RZConditionalVar) == RAZIX_CACHE_LINE_SIZE, "RZConditionalVar must be less than cache line");
    static_assert(alignof(RZConditionalVar) == RAZIX_CACHE_LINE_SIZE, "RZConditionalVar must be cache-line aligned");

}    // namespace Razix

#endif    // _RZ_SYNC_H_
