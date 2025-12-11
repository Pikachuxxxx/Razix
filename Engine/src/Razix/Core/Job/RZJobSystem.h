#ifndef _RZ_JOB_SYSTEM_H
#define _RZ_JOB_SYSTEM_H

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/thread.h"

#define RAZIX_JOBS_MAX_DEPENDENCIES      8
#define RAZIX_JOBS_INVALID_ID            0xFFFFFFFFFFFFFFFFull
#define RAZIX_MAX_WORKER_THREADS         8
#define RAZIX_JOB_NAME_MAX_CHARS         64
#define RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE 1024
#define RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE  256
#define RAZIX_WORKER_BUSY_THRESHOLD_JOBS 8    // If a worker has more than these many jobs in its local queue, it is considered busy and other workers will not attempt to steal from it

typedef struct rz_job rz_job;
void                  rz_job_func_cb(rz_job* pJob);

struct rz_job
{
    void (*pFunc)(rz_job* pJob);
    void*         pUserData;
    rz_atomic_u32 unfinishedJobs;
    rz_job*       pDeps[RAZIX_JOBS_MAX_DEPENDENCIES];
    u32           depCount;
    char          pName[RAZIX_JOB_NAME_MAX_CHARS];
};

typedef u64 rz_job_handle;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_local_job_queue
{
    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_job* pJobs[RAZIX_MAX_LOCAL_JOBS_QUEUE_SIZE];

    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 head;

    RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    rz_atomic_u32 tail;

} rz_local_job_queue;

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_global_job_queue
{
    rz_job* pJobs[RAZIX_MAX_GLOBAL_JOBS_QUEUE_SIZE];

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
    bool          isBusy;

} rz_worker;

typedef struct rz_job_system
{
    rz_global_job_queue globalQueue;
    rz_worker           workers[RAZIX_MAX_WORKER_THREADS];
    u32                 workerCount;
    rz_atomic_u32       shutdown;
    rz_atomic_u32       jobsInSystem;
    rz_atomic_u32       roundRobinWorkerIndex;
} rz_job_system;

extern RAZIX_TLS rz_worker* ptls_CurrentWorker;
static struct rz_job_system g_JobSystem;

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_JOB_SYSTEM_H
