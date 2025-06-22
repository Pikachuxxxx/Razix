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
#define RAZIX_MEMORY_MAX_ALLOCATION_SIZE 10 * 1024 * 1024 * 1024    // 10 GiB

/**
 * Maximum amount of dynamic pool memory that can be done at once
 */
#define RAZIX_MEMORY_MAX_DYNAMIC_ALLOCATION_SIZE 256 * 1024 * 1024    // 256 Mib

/**
 * Frame budget for dynamic and static allocations (default, can be customized)
 */
#define RAZIX_DEFAULT_FRAME_BUDGET 1 * 1024 * 1024 * 1024    // 1 Gib

/**
 * Minimum pool allocation of 64 kilobytes for efficient page size DMA copy
 */
#define RAZIX_MEMORY_MIN_POOL_ALLOC_SIZE 64 * 1024    // 64 Kib

/**
 * Memory size constants
 */
#define Gib(x) x * (1 << 30)
#define Mib(x) x * (1 << 20)
#define Kib(x) x * (1 << 10)

#define in_Gib(x) (x / (1 << 30))
#define in_Mib(x) (x / (1 << 20))
#define in_Kib(x) (x / 1024)
