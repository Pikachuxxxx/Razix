#include "TanuBrain.h"

#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/sync.h"
#include "Razix/Core/Utils/RZTime.h"

#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Platform HTTP helpers
// ---------------------------------------------------------------------------
#if defined(RAZIX_PLATFORM_WINDOWS)
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>
#    include <wininet.h>
#    pragma comment(lib, "wininet.lib")
#else
// POSIX: delegate to the system curl binary (available on Linux & macOS)
#    include <unistd.h>
#endif

// Internal buffer size for the full thoughts.txt download (~90 KB currently).
// Allocated once as a static buffer so we avoid repeated heap churn.
#define TANU_FETCH_BUF_SIZE (512 * 1024)    // 512 KB — headroom for future growth

// ---------------------------------------------------------------------------
// Internal state
// ---------------------------------------------------------------------------
static rz_time_stamp       s_LastFetchTime   = {0};
static bool                s_Initialized     = false;
static rz_atomic_u32       s_FetchInProgress = 0;    // 1 while worker is running

// Reusable download buffer — allocated once, used on the worker thread only
// (safe because we only ever have one fetch job in flight at a time).
static char s_FetchBuf[TANU_FETCH_BUF_SIZE];

// The worker writes the latest thought here; the game thread reads it.
static char                s_FetchedThought[TANU_BRAIN_MAX_THOUGHT_LEN] = {0};
static rz_critical_section s_ThoughtMutex   = {0};
static rz_atomic_u32       s_NewThoughtReady = 0;    // 1 when a fresh thought is waiting

// Job storage — one outstanding fetch at a time.
static rz_job      s_FetchJob     = {0};
static rz_job_cold s_FetchJobCold = {0};

// ---------------------------------------------------------------------------
// Platform HTTP GET — fills outBuf with raw content from the fixed tanu-brain
// URL.  The URL is a compile-time constant so there is no dynamic string
// interpolation on the POSIX path, eliminating any shell-injection risk.
// ---------------------------------------------------------------------------
static bool _tanu_http_get(char* outBuf, size_t bufSize)
{
    size_t totalRead = 0;
    bool   ok        = false;

#if defined(RAZIX_PLATFORM_WINDOWS)
    HINTERNET hInet = InternetOpenA(
        "Razix/TanuBrain/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL, NULL, 0);
    if (hInet) {
        HINTERNET hUrl = InternetOpenUrlA(
            hInet, TANU_BRAIN_THOUGHTS_RAW_URL, NULL, 0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE,
            0);
        if (hUrl) {
            DWORD bytesRead = 0;
            while (totalRead < bufSize - 1) {
                if (!InternetReadFile(hUrl,
                        outBuf + totalRead,
                        (DWORD)(bufSize - 1 - totalRead),
                        &bytesRead))
                    break;
                if (bytesRead == 0)
                    break;
                totalRead += (size_t)bytesRead;
            }
            outBuf[totalRead] = '\0';
            ok                = (totalRead > 0);
            InternetCloseHandle(hUrl);
        }
        InternetCloseHandle(hInet);
    }
#else
    // POSIX: delegate to the system curl binary.
    // The command string uses only the compile-time constant URL, so there is
    // no user-controlled input and no shell-injection risk.
    FILE* pipe = popen("curl -s --max-time 15 \"" TANU_BRAIN_THOUGHTS_RAW_URL "\"", "r");
    if (pipe) {
        totalRead         = fread(outBuf, 1, bufSize - 1, pipe);
        outBuf[totalRead] = '\0';
        ok                = (totalRead > 0);
        pclose(pipe);
    }
#endif

    return ok;
}

// ---------------------------------------------------------------------------
// Extract the latest timestamped thought from the thoughts.txt content.
// Each thought entry starts with "HH:MM: " (digits, colon, digits, colon).
// We find the last such line and return it as a trimmed string.
// ---------------------------------------------------------------------------
static bool _tanu_extract_latest_thought(const char* content, char* outThought, size_t maxLen)
{
    const char* lastTimestampedLine = NULL;
    const char* p                   = content;

    while (*p) {
        const char* nl      = strchr(p, '\n');
        size_t      lineLen = nl ? (size_t)(nl - p) : strlen(p);

        if (lineLen >= 5) {
            // Validate timestamp pattern: starts with digits then ':' then digits then ':'
            // e.g. "11:00:" or "09:13:"
            if (p[0] >= '0' && p[0] <= '9' &&
                p[1] >= '0' && p[1] <= '9' &&
                p[2] == ':' &&
                p[3] >= '0' && p[3] <= '9' &&
                p[4] >= '0' && p[4] <= '9') {
                lastTimestampedLine = p;
            }
        }

        p = nl ? nl + 1 : p + lineLen;
        if (!nl)
            break;
    }

    if (!lastTimestampedLine)
        return false;

    const char* eol     = strchr(lastTimestampedLine, '\n');
    size_t      len     = eol ? (size_t)(eol - lastTimestampedLine) : strlen(lastTimestampedLine);
    size_t      copyLen = (len < maxLen - 1) ? len : maxLen - 1;

    if (copyLen == 0)
        return false;

    strncpy(outThought, lastTimestampedLine, copyLen);
    outThought[copyLen] = '\0';

    // Strip trailing carriage return if present
    size_t outLen = strlen(outThought);
    while (outLen > 0 && (outThought[outLen - 1] == '\r' || outThought[outLen - 1] == '\n'))
        outThought[--outLen] = '\0';

    return (strlen(outThought) > 0);
}

// ---------------------------------------------------------------------------
// Job callback — runs on a worker thread
// ---------------------------------------------------------------------------
static void _tanu_brain_fetch_job_cb(rz_job* pJob)
{
    (void) pJob;

    memset(s_FetchBuf, 0, sizeof(s_FetchBuf));

    bool fetched = _tanu_http_get(s_FetchBuf, sizeof(s_FetchBuf));

    if (fetched) {
        char thought[TANU_BRAIN_MAX_THOUGHT_LEN] = {0};
        if (_tanu_extract_latest_thought(s_FetchBuf, thought, sizeof(thought))) {
            rz_critical_section_lock(&s_ThoughtMutex);
            strncpy(s_FetchedThought, thought, sizeof(s_FetchedThought) - 1);
            s_FetchedThought[sizeof(s_FetchedThought) - 1] = '\0';
            rz_atomic32_store(&s_NewThoughtReady, 1, RZ_MEMORY_ORDER_RELEASE);
            rz_critical_section_unlock(&s_ThoughtMutex);
        }
    }

    // Signal job is done — must be the last write before the game thread reads it
    rz_atomic32_store(&s_FetchInProgress, 0, RZ_MEMORY_ORDER_RELEASE);
}

// ---------------------------------------------------------------------------
// Internal helper: prepare and submit a fetch job
// ---------------------------------------------------------------------------
static void _tanu_submit_fetch_job(void)
{
    memset(&s_FetchJob, 0, sizeof(s_FetchJob));
    memset(&s_FetchJobCold, 0, sizeof(s_FetchJobCold));
    strncpy(s_FetchJobCold.pName, "TanuBrainFetch", RAZIX_JOB_NAME_MAX_CHARS - 1);
    s_FetchJob.hot.pFunc     = _tanu_brain_fetch_job_cb;
    s_FetchJob.hot.pUserData = NULL;
    s_FetchJob.pCold         = &s_FetchJobCold;

    rz_atomic32_store(&s_FetchInProgress, 1, RZ_MEMORY_ORDER_RELEASE);
    rz_job_system_submit_job(&s_FetchJob);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
void tanu_brain_init(void)
{
    s_ThoughtMutex  = rz_critical_section_create();
    s_LastFetchTime = rz_time_now();
    s_Initialized   = true;

    rz_atomic32_store(&s_FetchInProgress, 0, RZ_MEMORY_ORDER_RELEASE);
    rz_atomic32_store(&s_NewThoughtReady, 0, RZ_MEMORY_ORDER_RELEASE);

    // Kick off the first fetch immediately so a thought appears early on.
    _tanu_submit_fetch_job();
}

void tanu_brain_update(void)
{
    if (!s_Initialized)
        return;

    // ── Check if a fresh thought arrived from the worker ──────────────────
    if (rz_atomic32_load(&s_NewThoughtReady, RZ_MEMORY_ORDER_ACQUIRE)) {
        char thought[TANU_BRAIN_MAX_THOUGHT_LEN] = {0};

        rz_critical_section_lock(&s_ThoughtMutex);
        strncpy(thought, s_FetchedThought, sizeof(thought) - 1);
        thought[sizeof(thought) - 1] = '\0';
        rz_atomic32_store(&s_NewThoughtReady, 0, RZ_MEMORY_ORDER_RELEASE);
        rz_critical_section_unlock(&s_ThoughtMutex);

#if TANU_BRAIN_PRINT_IN_DIST_BUILD
        // printf is used intentionally so the thought surfaces even when
        // RAZIX_GOLD_MASTER strips RAZIX_CORE_TRACE.
        printf("[Tanu's thought] %s\n", thought);
        fflush(stdout);
#else
        RAZIX_CORE_TRACE("[Tanu's thought] {0}", thought);
#endif
    }

    // ── Schedule the next fetch when the interval has elapsed ─────────────
    if (!rz_atomic32_load(&s_FetchInProgress, RZ_MEMORY_ORDER_ACQUIRE)) {
        rz_time_stamp now     = rz_time_now();
        double        elapsed = rz_get_elapsed_ms(s_LastFetchTime, now);

        if (elapsed >= (double) TANU_THOUGHT_FETCH_INTERVAL_MS) {
            s_LastFetchTime = now;
            _tanu_submit_fetch_job();
        }
    }
}

void tanu_brain_shutdown(void)
{
    if (!s_Initialized)
        return;

    // Drain any in-flight fetch before tearing down
    if (rz_atomic32_load(&s_FetchInProgress, RZ_MEMORY_ORDER_ACQUIRE)) {
        rz_job_system_wait_for_job(&s_FetchJob);
    }

    rz_critical_section_destroy(&s_ThoughtMutex);
    s_Initialized = false;
}
