#pragma once


/**
 * Memory config macros such as to track context switches etc.
 */

/**
 * Debug mode configuration settings
 */
#if defined(RAZIX_DEBUG) || defined(RAZIX_RELEASE)
    // Allocations in debug mode could be slow, this is will enable memory tracking and analysis on a deeper level
    #define RAZIX_MEMORY_DEBUG
#endif


/**
 * Enable Heap allocation tracking
 */
#define RAZIX_MEMORY_TRACK_HEAP_ENABLE 0

/**
 * Max amount of memory that can be allocated by all the pools ever, for more we need special request by the engine
 */
#define RAZIX_MEMORY_MAX_ALLOCATION 10737418240 // 10 GB in bytes

/**
 * Minimum pool allocation 
 */