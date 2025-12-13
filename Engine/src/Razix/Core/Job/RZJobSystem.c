#include "RZJobSystem.h"

#include "Razix/Core/Log/RZLog.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/thread.h"

#include <string.h>    // for memset

RAZIX_TLS rz_worker* pTLS_CurrentWorker = NULL;
static rz_job_system g_JobSystem        = {0};

static void _rz_worker_run_(void* pUserData)
{
    rz_worker* pWorker = (rz_worker*) pUserData;
    pTLS_CurrentWorker = pWorker;    // store in TLS

    while (1) {
        if (rz_atomic32_load(&pWorker->shutdown, RZ_MEMORY_ORDER_ACQUIRE)) {
            RAZIX_CORE_TRACE("Exiting worker thread...");
            //rz_thread_exit(pTLS_CurrentWorker->threadHandle);
            break;    // Exit the loop and thread
        }

        // Scheduling logic
        // check local jobsInLocalQueue
        // check global queue
        // TODO: steam from other workers
        //
        // spin for N cycles and then yield, never sleep
        // repeat!
    }
}

void rz_job_system_startup(u32 workerCount)
{
    RAZIX_CORE_INFO("[JobSystem] Starting up Job System...");
    RAZIX_CORE_ASSERT(workerCount <= RAZIX_MAX_WORKER_THREADS, "Requested more worker threads than the maximum supported!");

    memset(&g_JobSystem, 0, sizeof(rz_job_system));
    g_JobSystem.workerCount = workerCount;

    // Time to create some workers
    for (u32 i = 0; i < workerCount; ++i) {
        rz_worker* pWorker = &g_JobSystem.workers[i];
        memset(pWorker, 0, sizeof(rz_worker));
        pWorker->handle = i;

        char threadName[32];
        rz_snprintf(threadName, sizeof(threadName), "rz_worker_%u", i);

        pWorker->threadHandle = rz_thread_create(
            threadName,
            RZ_THREAD_PRIORITY_NORMAL,
            RZ_THREAD_AFFINITY_WORKER,
            _rz_worker_run_,
            (void*) pWorker);
    }

    RAZIX_CORE_INFO("[JobSystem] Job System started with %u workers", workerCount);
}

void rz_job_system_shutdown(void)
{
    RAZIX_CORE_INFO("[JobSystem] Shutting down Job System...");

    // close and exit all worker threads
    for (u32 i = 0; i < g_JobSystem.workerCount; ++i) {
        rz_worker* pWorker = &g_JobSystem.workers[i];
        rz_atomic32_store(&pWorker->shutdown, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    for (u32 i = 0; i < g_JobSystem.workerCount; ++i) {
        rz_worker* pWorker = &g_JobSystem.workers[i];
        rz_thread_join(pWorker->threadHandle);
    }
}
