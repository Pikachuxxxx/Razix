#include "RZJobSystem.h"

RAZIX_TLS rz_worker* ptls_CurrentWorker = NULL;
static rz_job_system g_JobSystem;
