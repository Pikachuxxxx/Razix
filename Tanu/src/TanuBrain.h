#pragma once

// clang-format off

// ============================================================================
// Tanu Brain — thought feed from tanu-brain GitHub repo
// https://github.com/Pikachuxxxx/tanu-brain
//
// Every TANU_THOUGHT_FETCH_INTERVAL_MS milliseconds a worker job fetches the
// latest entry from tanu-corner/thoughts.txt and prints it to stdout via
// printf so it surfaces even in Gold Master / distribution builds.
// ============================================================================

// ---------------------------------------------------------------------------
// Configuration — change these without touching the implementation
// ---------------------------------------------------------------------------

/** Interval between consecutive thought fetches (default: 90 minutes in ms). */
#define TANU_THOUGHT_FETCH_INTERVAL_MS  (90ULL * 60ULL * 1000ULL)

/** Maximum number of characters kept for a single thought (including NUL). */
#define TANU_BRAIN_MAX_THOUGHT_LEN      2048

/** Set to 1 to always print Tanu's thought, even in Gold Master / dist builds.
 *  Set to 0 to suppress output in those configurations. */
#define TANU_BRAIN_PRINT_IN_DIST_BUILD  1

/** Raw URL for the tanu-brain thoughts file hosted on GitHub. */
#define TANU_BRAIN_THOUGHTS_RAW_URL \
    "https://raw.githubusercontent.com/Pikachuxxxx/tanu-brain/master/tanu-corner/thoughts.txt"

// ---------------------------------------------------------------------------
// Public C API (callable from C++ via extern "C")
// ---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the Tanu Brain module.
 * Must be called AFTER rz_job_system_startup().
 * Submits an initial fetch job so a thought is ready early on.
 */
void tanu_brain_init(void);

/**
 * Tick the Tanu Brain module — call every frame from the game / main thread.
 *  - Checks whether a completed fetch result is waiting; if so, prints it.
 *  - Checks whether TANU_THOUGHT_FETCH_INTERVAL_MS has elapsed since the last
 *    fetch; if so, submits a new worker job to fetch the latest thought.
 */
void tanu_brain_update(void);

/**
 * Shut down the Tanu Brain module.
 * Waits for any in-flight fetch job to complete and releases resources.
 */
void tanu_brain_shutdown(void);

#ifdef __cplusplus
}
#endif
