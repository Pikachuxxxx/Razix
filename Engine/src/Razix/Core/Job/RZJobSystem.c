#include "RZJobSystem.h"

#include "Razix/Core/Log/RZLog.h"

#include <string.h> // for memset

RAZIX_TLS rz_worker* ptls_CurrentWorker = NULL;
static rz_job_system g_JobSystem = {0};

void rz_job_system_startup(u32 workerCount)
{
    RAZIX_CORE_INFO("[JobSystem] Starting up Job System...");
    RAZIX_CORE_ASSERT(workerCount <= RAZIX_MAX_WORKER_THREADS, "Requested more worker threads than the maximum supported!");

    memset(&g_JobSystem, 0, sizeof(rz_job_system));
    g_JobSystem.workerCount = workerCount;
}

void rz_job_system_shutdown(void)
{
    RAZIX_CORE_INFO("[JobSystem] Shutting down Job System...");
}
