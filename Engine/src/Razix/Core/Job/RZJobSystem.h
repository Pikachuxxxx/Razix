#ifndef _RZ_JOB_SYSTEM_H
#define _RZ_JOB_SYSTEM_H

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/thread.h"

#define RAZIX_JOBS_MAX_DEPENDENCIES      8
#define RAZIX_JOBS_INVALID_ID            0xFFFFFFFFFFFFFFFFull
#define RAZIX_WORKER_SPIN_CYCLES         400
#define RAZIX_MAX_WORKER_THREADS         8
#define RAZIX_JOB_NAME_MAX_CHARS         64
#define RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE 1024
#define RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE  256
#define RAZIX_WORKER_BUSY_THRESHOLD_JOBS 128    // If a worker has more than these many jobs in its local queue, it is considered busy and other workers will not attempt to steal from it

typedef struct rz_job rz_job;
void                  rz_job_func_cb(rz_job* pJob);

// TODO: Profile and tune cache line alignments for better performance
// TODO: Add manual padding where necessary for cache line alignment to avoid false sharing

typedef struct rz_job_hot
{
    void (*pFunc)(rz_job* pJob);
    void*         pUserData;
    u32           blockedByCount;
    rz_atomic_u32 isExecuted;
} rz_job_hot;

typedef struct rz_job_cold
{
    // Jobs this job is blocked on (parents - must complete before this runs)
    rz_job* pBlockedOn[RAZIX_JOBS_MAX_DEPENDENCIES];
    // Jobs blocked by this job (children - waiting for this to complete)
    rz_job* pBlockedBy[RAZIX_JOBS_MAX_DEPENDENCIES];
    u32     blockedOnCount;
    u32     _pad0;
    char    pName[RAZIX_JOB_NAME_MAX_CHARS];
} rz_job_cold;

struct rz_job
{
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_job_hot hot;
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_job_cold* pCold;
};

typedef u64 rz_job_handle;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_local_job_queue
{
    rz_job* ppJobs[RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE];

    // used internally by the owner worker, hence no false sharing and need to be on separate thread
    u32 tail;

    // Put the write index on its own cache line
    // Shared head for stealing
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 head;

} rz_local_job_queue;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_global_job_queue
{
    rz_job* ppJobs[RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE];

    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 head;

    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 tail;
} rz_global_job_queue;

typedef u64 rz_worker_handle;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_worker
{
    rz_thread_handle threadHandle;
    rz_worker_handle handle;

    // Put the queue on its own cache line
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_local_job_queue localQueue;

    // Put isBusy / load flags separately
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 jobsInLocalQueue;
    rz_atomic_u32 jobsInWorker;
    rz_atomic_u32 shutdown;
    bool          isBusy;

} rz_worker;

typedef struct rz_job_system
{
    rz_global_job_queue globalQueue;
    rz_worker           workers[RAZIX_MAX_WORKER_THREADS];
    u32                 workerCount;
    rz_atomic_u32       jobsInSystem;
    rz_atomic_u32       roundRobinWorkerIndex;
} rz_job_system;

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    // Job System Public API
    RAZIX_API void rz_job_system_startup(u32 workerCount);
    RAZIX_API void rz_job_system_shutdown(void);

    RAZIX_API void rz_job_system_submit_job(rz_job* pJob);
    RAZIX_API void rz_job_system_worker_spawn_job(rz_job* pJob);
    RAZIX_API void rz_job_system_add_dependency(rz_job* pJob, rz_job* pDependency);
    RAZIX_API void rz_job_system_wait_for_job(rz_job* pJob);
    RAZIX_API void rz_job_system_wait_for_all(void);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_JOB_SYSTEM_H
