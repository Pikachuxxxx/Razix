#include "RZJobSystem.h"

#include "Razix/Core/Log/RZLog.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/thread.h"

#include <string.h>    // FIXME: for memset, this fuckery needs to be addressed we need a SIMD rz_memset soon

#if defined(RAZIX_PLATFORM_LINUX) || defined(RAZIX_PLATFORM_MACOS)
    #include <sched.h>
#elif defined RAZIX_PLATFORM_WINDOWS
    #include <windows.h>
#endif

#include <stdio.h>    // for temp testing only

//-----------------------------------------------------------------------------
RAZIX_TLS rz_worker* pTLS_CurrentWorker = NULL;
static rz_job_system g_JobSystem        = {0};
//-----------------------------------------------------------------------------

static void _rz_global_queue_push_(rz_job* pJob)
{
    if (pJob == NULL) return;

    u32 head     = rz_atomic32_load(&g_JobSystem.globalQueue.head, RZ_MEMORY_ORDER_ACQUIRE);
    u32 tail     = rz_atomic32_load(&g_JobSystem.globalQueue.tail, RZ_MEMORY_ORDER_ACQUIRE);
    u32 nextTail = (tail + 1) & (RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE - 1);

    if (nextTail == head) {
        RAZIX_CORE_ERROR("[JobSystem] Global queue is full, failed to add Job! ==> CATASTROPIC EFFECT, JOB LOST!");
        return;
    }

    g_JobSystem.globalQueue.ppJobs[tail] = pJob;
    rz_atomic32_store(&g_JobSystem.globalQueue.tail, nextTail, RZ_MEMORY_ORDER_RELEASE);
}

static rz_job* _rz_global_queue_pop_()
{
    while (1) {
        u32 head = rz_atomic32_load(&g_JobSystem.globalQueue.head, RZ_MEMORY_ORDER_RELAXED);
        u32 tail = rz_atomic32_load(&g_JobSystem.globalQueue.tail, RZ_MEMORY_ORDER_ACQUIRE);

        if (head == tail)
            return NULL;

        u32 nextHead = (head + 1) & (RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE - 1);

        if (rz_atomic32_cas(&g_JobSystem.globalQueue.head, head, nextHead, RZ_MEMORY_ORDER_ACQ_REL))
            return g_JobSystem.globalQueue.ppJobs[head];
    }
}

/**
 * pushes a job into worker local queue, using the head
 * Either jobs can be spawned by the worker itself
 * or external threads can push into this by incrementing the head
 */
static void _rz_worker_local_queue_push_(rz_worker* pWorker, rz_job* pJob)
{
    if (pJob == NULL || pWorker == NULL) return;

    if (pWorker != NULL) {
        u32 tail     = rz_atomic32_load(&pWorker->localQueue.tail, RZ_MEMORY_ORDER_ACQUIRE);
        u32 head     = rz_atomic32_load(&pWorker->localQueue.head, RZ_MEMORY_ORDER_ACQUIRE);
        u32 nextTail = (tail + 1) & (RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE - 1);

        if (nextTail == head) {
            RAZIX_CORE_WARN("[JobSystem] Worker local queue is full, pushing job onto the global queue");
            _rz_global_queue_push_(pJob);
            return;
        }

        pWorker->localQueue.ppJobs[tail] = pJob;
        pWorker->localQueue.tail         = nextTail;
        rz_atomic32_store(&pWorker->localQueue.tail, nextTail,RZ_MEMORY_ORDER_RELEASE);
        rz_atomic32_increment(&pWorker->jobsInLocalQueue, RZ_MEMORY_ORDER_RELEASE);
    }
}

static rz_job* _rz_worker_local_queue_pop_(rz_worker* pWorker)
{
    if (pWorker == NULL) return NULL;

    u32 tail = pWorker->localQueue.tail;
    u32 head = rz_atomic32_load(&pWorker->localQueue.head, RZ_MEMORY_ORDER_ACQUIRE);

    if (tail == head) {
        return NULL;    // Queue is empty
    }

    u32 newTail              = (tail - 1) & (RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE - 1);
    pWorker->localQueue.tail = newTail;
    rz_atomic32_decrement(&pWorker->jobsInLocalQueue, RZ_MEMORY_ORDER_RELEASE);

    return pWorker->localQueue.ppJobs[newTail];
}

static rz_worker* _rz_find_victim_worker_to_steal_from_()
{
    // u32 maxJobs = 0;
    rz_worker* pVictim = NULL;

    // Iterate through all workers except current one
    for (u32 i = 0; i < g_JobSystem.workerCount; ++i) {
        rz_worker* pWorker = &g_JobSystem.workers[i];

        // Skip self
        if (pWorker == pTLS_CurrentWorker) {
            continue;
        }

        u32 jobsInQueue = rz_atomic32_load(&pWorker->jobsInLocalQueue, RZ_MEMORY_ORDER_ACQUIRE);

        // Find the one that is above threshold and most busy of all
        // TODO: Check if it's better to check all worker or find the first one that's atleast some busy than the threshold
        if (jobsInQueue > RAZIX_WORKER_BUSY_THRESHOLD_JOBS /* && jobsInQueue > maxJobs */) {
            // maxJobs = jobsInQueue;
            pVictim = pWorker;
            return pVictim;    // TODO: Disable if we checking for the most busiest worker to steal from
        }
    }

    return pVictim;
}

static rz_worker* _rz_find_best_worker_to_submit_to_()
{
    u32        minJobs     = UINT32_MAX;
    rz_worker* pBestWorker = NULL;

    u32 startIdx = rz_atomic32_increment(&g_JobSystem.roundRobinWorkerIndex, RZ_MEMORY_ORDER_RELAXED) % g_JobSystem.workerCount;

    for (u32 i = 0; i < g_JobSystem.workerCount; ++i) {
        u32        idx     = (startIdx + i) % g_JobSystem.workerCount;
        rz_worker* pWorker = &g_JobSystem.workers[idx];

        u32 jobsInQueue = rz_atomic32_load(&pWorker->jobsInLocalQueue, RZ_MEMORY_ORDER_ACQUIRE);

        if (jobsInQueue < RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE - 1) {
            if (jobsInQueue < minJobs) {
                minJobs     = jobsInQueue;
                pBestWorker = pWorker;
            }

            if (minJobs == 0) {
                break;
            }
        }
    }

    return pBestWorker;
}

static rz_job* _rz_worker_steal_(rz_worker* pVictimWorker)
{
    if (pVictimWorker == NULL) return NULL;

    u32 head    = rz_atomic32_load(&pVictimWorker->localQueue.head, RZ_MEMORY_ORDER_ACQUIRE);
    u32 newHead = (head + 1) & (RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE - 1);

    // if CAS success we stole the job if not someone else stole it
    if (!rz_atomic32_cas(&pVictimWorker->localQueue.head, head, newHead, RZ_MEMORY_ORDER_ACQ_REL)) {
        return NULL;
    }

    rz_atomic32_decrement(&pVictimWorker->jobsInLocalQueue, RZ_MEMORY_ORDER_RELEASE);
    return pVictimWorker->localQueue.ppJobs[head];
}

static inline void _rz_job_execute_(rz_job* pJob)
{
    if (pJob == NULL) return;

    pJob->hot.pFunc(pJob->hot.pUserData);

    rz_atomic32_store(&pJob->hot.isExecuted, 1, RZ_MEMORY_ORDER_RELEASE);

    // Unblock all jobs that were blocked by this one
    for (u32 i = 0; i < pJob->hot.blockedByCount; ++i) {
        rz_job* pDependent = pJob->pCold->pBlockedBy[i];
        u32 old = rz_atomic32_decrement(&pDependent->hot.blockedByCount, RZ_MEMORY_ORDER_RELEASE);
        if(old == 1)
            _rz_worker_local_queue_push_(pTLS_CurrentWorker, pJob);
    }
    rz_atomic32_decrement(&g_JobSystem.jobsInSystem, RZ_MEMORY_ORDER_RELEASE);
}

static void _rz_worker_run_(void* pUserData)
{
    rz_worker* pWorker = (rz_worker*) pUserData;
    pTLS_CurrentWorker = pWorker;

    u32 spinCount = 0;

    while (1) {
        if (rz_atomic32_load(&pWorker->shutdown, RZ_MEMORY_ORDER_ACQUIRE)) {
            break;    // Exit the loop and thread
        }

        rz_job* pJob = NULL;

        // Scheduling logic
        // PRIORITY 1: check local jobsInLocalQueue
        pJob = _rz_worker_local_queue_pop_(pTLS_CurrentWorker);
        if (pJob) {
            spinCount = 0;
            _rz_job_execute_(pJob);
            continue;
        }

        // PRIORITY 2: Steal from other workers
        rz_worker* pVictimWorker = _rz_find_victim_worker_to_steal_from_();
        if (pVictimWorker) {
            pJob = _rz_worker_steal_(pVictimWorker);
            if (pJob) {
                spinCount = 0;
                _rz_job_execute_(pJob);
                continue;
            }
        }

        // PRIORITY 3: steal from global queue as last resort
        pJob = _rz_global_queue_pop_();
        if (pJob) {
            spinCount = 0;
            _rz_job_execute_(pJob);
            continue;
        }

        if (spinCount < RAZIX_WORKER_SPIN_CYCLES) {
            RAZIX_BUSY_WAIT();
            spinCount++;
        } else {
            RAZIX_YIELD();
            spinCount = 0;
        }
    }

    rz_thread_exit(pTLS_CurrentWorker->threadHandle);
}

//-----------------------------------------------------------------------------
// Public API
void rz_job_system_startup(u32 workerCount)
{
    RAZIX_CORE_INFO("[JobSystem] Starting up Job System...");
    RAZIX_CORE_ASSERT(workerCount <= RAZIX_MAX_WORKER_THREADS, "Requested more worker threads than the maximum supported!");

    memset(&g_JobSystem, 0, sizeof(rz_job_system));
    g_JobSystem.workerCount = workerCount;

    rz_atomic32_store(&g_JobSystem.jobsInSystem, 0, RZ_MEMORY_ORDER_RELEASE);
    rz_atomic32_store(&g_JobSystem.globalQueue.head, 0, RZ_MEMORY_ORDER_RELEASE);
    rz_atomic32_store(&g_JobSystem.globalQueue.tail, 0, RZ_MEMORY_ORDER_RELEASE);

    // Time to create some workers
    for (u32 i = 0; i < workerCount; ++i) {
        rz_worker* pWorker = &g_JobSystem.workers[i];
        memset(pWorker, 0, sizeof(rz_worker));
        rz_atomic32_store(&pWorker->jobsInLocalQueue, 0, RZ_MEMORY_ORDER_RELAXED);
        rz_atomic32_store(&pWorker->jobsInWorker, 0, RZ_MEMORY_ORDER_RELAXED);
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

void rz_job_system_submit_job(rz_job* pJob)
{
    if (pJob == NULL) return;

    rz_atomic32_increment(&g_JobSystem.jobsInSystem, RZ_MEMORY_ORDER_RELEASE);
    rz_atomic32_store(&pJob->hot.isExecuted, 0, RZ_MEMORY_ORDER_RELEASE);

    // rz_worker* pWorker = _rz_find_best_worker_to_submit_to_();
    // if (pWorker) {
    //     _rz_worker_local_queue_push_(pWorker, pJob);
    // } else {
        _rz_global_queue_push_(pJob);
    // }
}

void rz_job_system_worker_spawn_job(rz_job* pJob)
{
    if (pJob == NULL) return;

    // Only submit on local thread
    if (pTLS_CurrentWorker != NULL) {
        rz_atomic32_increment(&g_JobSystem.jobsInSystem, RZ_MEMORY_ORDER_RELEASE);
        rz_atomic32_store(&pJob->hot.isExecuted, 0, RZ_MEMORY_ORDER_RELEASE);
        _rz_worker_local_queue_push_(pTLS_CurrentWorker, pJob);
    } else {
        RAZIX_CORE_ERROR("[JobSystem] Trying to spawn from non-worker, this is wrong!");
        RAZIX_DEBUG_BREAK();
    }
}

void rz_job_system_add_dependency(rz_job* pJob, rz_job* pDependency)
{
    if (pJob == NULL || pDependency == NULL) return;

    if (pJob->hot.blockedOnCount >= RAZIX_JOBS_MAX_DEPENDENCIES) {
        RAZIX_CORE_WARN("[JobSystem] Job blockedOn limit reached!");
        return;
    }

    if (pDependency->hot.blockedByCount >= RAZIX_JOBS_MAX_DEPENDENCIES) {
        RAZIX_CORE_WARN("[JobSystem] Job blockedBy limit reached!");
        return;
    }

    pJob->pCold->pBlockedOn[pJob->hot.blockedOnCount++]               = pDependency;
    pDependency->pCold->pBlockedBy[pDependency->hot.blockedByCount++] = pJob;
}

void rz_job_system_wait_for_job(rz_job* pJob)
{
    if (pJob == NULL) return;

    while (!rz_atomic32_load(&pJob->hot.isExecuted, RZ_MEMORY_ORDER_ACQUIRE))
        RAZIX_YIELD();
}

void rz_job_system_wait_for_all(void)
{
    while (rz_atomic32_load(&g_JobSystem.jobsInSystem, RZ_MEMORY_ORDER_ACQUIRE) > 0) {
        RAZIX_YIELD();
    }
}
