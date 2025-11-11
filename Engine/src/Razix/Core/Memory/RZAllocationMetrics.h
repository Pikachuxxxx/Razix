#ifndef _RZ_ALLOC_METRICS_H
#define _RZ_ALLOC_METRICS_H

#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * Used to tag memory to create cluster pools
 * 
 * Note:- Multiple types can be OR-ed together for shared allocation however it's recommended
 * only to OR common tags and Threads for optimal usage
 */
    typedef enum
    {
        // Common Tags
        RZ_MEM_TAG_GRAPHICS,
        RZ_MEM_TAG_CORE,
        RZ_MEM_TAG_APPLICATION,
        RZ_MEM_TAG_SCENE,
        RZ_MEM_TAG_PHYSICS,
        RZ_MEM_TAG_SCRIPTING,
        RZ_MEM_TAG_COMMON,
        RZ_MEM_TAG_UNKNOWN,
        // VERY IMPORTANT TAGS !!!
        RZ_MEM_TAG_THREAD_GLOBAL,
        RZ_MEM_TAG_THREAD_SHARED
    } RZMemoryTag;

    typedef enum
    {
        RZ_MEM_LIFE_TRANSIENT,    // Per-frame allocations will use a Bump/Ring based frame allocator
        RZ_MEM_LIFE_PERSISTENT    // These kind of allocations use a Pool based on memory Tag and alloc properties
    } RZMemoryLife;

    /**
 * Allocations will be divided into clusters depending on the category of Allocations such as GRAPHICS, SCENE, PHYSICS, SHARED_THREAD pools etc
 * Each cluster will have cells that contain same types for fixed aligned Allocations ex. a GRAPHICS cluster will have cells for VertexBuffer, IndexBuffer etc.
 */

    typedef struct
    {
        const char* name;
        u32         line;
        const char* file;
        RZMemoryTag tag;
        void*       address;
        sz          size;
    } RZMemAllocInfo;

    /**
 * Iterator for tracking memory allocations
 * Allows sequential traversal through all recorded allocations
 */
    typedef struct RZMemAllocInfoIterator
    {
        void* allocations;     // Pointer to allocation array (void* for opaque type)
        u32   currentIndex;    // Current position in iteration
        u32   totalCount;      // Total number of allocations
        u32   capacity;        // Capacity of allocations array
    } RZMemAllocInfoIterator;

    typedef struct
    {
        RZMemAllocInfo base;
        u32            clusterID;
        u32            clusterSize;
    } RZMemClusterAllocInfo;

    typedef struct
    {
        RZMemClusterAllocInfo base;
        u32                   elementSize;
        u32                   elementsCount;
        u32                   cellID;
        u8                    cellAlignment;
    } RZMemCellAllocInfo;

    /**
 * Memory view containing snapshot of memory state
 * Used for debugging and profiling memory usage
 */
    typedef struct RZMemView
    {
        RZMemAllocInfo*        allocations;           // Array of allocations
        RZMemClusterAllocInfo* clusterAllocations;    // Array of cluster allocations
        RZMemCellAllocInfo*    cellAllocations;       // Array of cell allocations
        u32                    allocationCount;       // Number of allocations
        u32                    clusterCount;          // Number of clusters
        u32                    cellCount;             // Number of cells
        u32                    capacity;              // Allocated capacity
        sz                     totalAllocated;        // Total bytes allocated in this view
        u64                    timestamp;             // Timestamp of snapshot (milliseconds)
    } RZMemView;

    /**
 * Memory allocation snapshot for tracking allocations over time
 * Stores and manages allocation metadata
 */
    typedef struct RZMemAllocSnapshot
    {
        RZMemAllocInfo*        allocations;           // Array of all allocations
        RZMemClusterAllocInfo* clusterAllocations;    // Array of cluster allocations
        RZMemCellAllocInfo*    cellAllocations;       // Array of cell allocations
        u32                    allocationCount;       // Current number of allocations
        u32                    clusterCount;          // Current number of clusters
        u32                    cellCount;             // Current number of cells
        u32                    capacity;              // Capacity of arrays
        sz                     totalAllocated;        // Total bytes currently allocated
        u64                    snapshotTime;          // Timestamp when snapshot was taken
        RZMemoryTag            filterTag;             // Filter by tag (-1 for all)
        bool                   isActive;              // Whether snapshot is actively recording
    } RZMemAllocSnapshot;

    /**
 * Individual memory tracking element
 * Represents a single tracked memory allocation with metadata
 */
    typedef struct RZMemInfoElement
    {
        RZMemAllocInfo allocInfo;           // Allocation information
        u64            allocationTime;      // Time of allocation (milliseconds since epoch)
        u64            deallocationTime;    // Time of deallocation (0 if still allocated)
        RZMemoryLife   lifetime;            // Lifetime category of this allocation
        u32            allocationID;        // Unique ID for this allocation
        u32            parentClusterID;     // ID of parent cluster (if applicable)
        bool           isAllocated;         // Whether currently allocated
        u8             _pad0[7];            // Padding for alignment
    } RZMemInfoElement;

    /**
 * Statistics for memory allocator
 * Tracks aggregated memory usage and allocation counts
 */
    typedef struct
    {
        sz  allocatedBytes;          // Currently allocated bytes
        sz  totalBytes;              // Total capacity
        sz  peakAllocatedBytes;      // Peak allocated bytes
        u32 allocationsCount;        // Current allocation count
        u32 peakAllocationsCount;    // Peak allocation count
        u32 deallocationCount;       // Total deallocations
        u32 allocationID;            // Next allocation ID to assign
    } RZMemAllocatorStats;

    void rz_memalloc_stats_add(RZMemAllocatorStats* stats, sz alloc);
    void rz_memalloc_stats_remove(RZMemAllocatorStats* stats, sz alloc);
    f32  rz_memalloc_stats_get_utilization(const RZMemAllocatorStats* stats);
    sz   rz_memalloc_stats_get_available(const RZMemAllocatorStats* stats);
    void rz_memalloc_stats_reset(RZMemAllocatorStats* stats);
    void rz_memalloc_stats_init(RZMemAllocatorStats* stats, sz total_bytes);
    void rz_memalloc_iterator_init(RZMemAllocInfoIterator* iterator, void* allocations, u32 count, u32 capacity);
    bool rz_memalloc_iterator_has_next(const RZMemAllocInfoIterator* iterator);
    void rz_memalloc_iterator_next(RZMemAllocInfoIterator* iterator);
    void rz_memalloc_iterator_reset(RZMemAllocInfoIterator* iterator);
    u32  rz_memalloc_iterator_index(const RZMemAllocInfoIterator* iterator);

    void rz_memalloc_view_init(RZMemView* view, u32 capacity);
    void rz_memalloc_view_destroy(RZMemView* view);
    void rz_memalloc_view_clear(RZMemView* view);
    sz   rz_memalloc_view_get_total(const RZMemView* view);

    void rz_memalloc_snapshot_init(RZMemAllocSnapshot* snapshot, u32 capacity);
    void rz_memalloc_snapshot_destroy(RZMemAllocSnapshot* snapshot);
    void rz_memalloc_snapshot_add_alloc(RZMemAllocSnapshot* snapshot, const RZMemAllocInfo* alloc_info);
    void rz_memalloc_snapshot_add_cluster(RZMemAllocSnapshot* snapshot, const RZMemClusterAllocInfo* cluster_info);
    void rz_memalloc_snapshot_add_cell(RZMemAllocSnapshot* snapshot, const RZMemCellAllocInfo* cell_info);
    bool rz_memalloc_snapshot_capture(RZMemAllocSnapshot* snapshot, RZMemView* view);
    void rz_memalloc_snapshot_display(const RZMemAllocSnapshot* snapshot);
    void rz_memalloc_snapshot_display_view(const RZMemView* view);
    void rz_memalloc_snapshot_set_filter(RZMemAllocSnapshot* snapshot, RZMemoryTag tag);
    void rz_memalloc_snapshot_clear(RZMemAllocSnapshot* snapshot);

    void rz_memalloc_element_init(RZMemInfoElement* element, const RZMemAllocInfo* alloc_info, RZMemoryLife lifetime, u32 alloc_id);
    void rz_memalloc_element_mark_deallocated(RZMemInfoElement* element);
    u64  rz_memalloc_element_get_lifetime(const RZMemInfoElement* element);
    bool rz_memalloc_element_is_allocated(const RZMemInfoElement* element);

#ifdef __cplusplus
}
#endif

#endif    // _RZ_ALLOC_METRICS_H