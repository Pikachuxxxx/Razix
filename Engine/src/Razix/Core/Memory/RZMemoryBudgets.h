#pragma once

#include "Razix/Core/RZDepartments.h"

#include "Razix/Core/Containers/string.h"

// TODO: Memory budgets and frame time budgets cannot exist together, because they don't have a 1:1 mapping
// For ex. memory for textures is shared across different departments but frame time is not, a texture loaded for environment department can be used by character department too
// Same memory pool used by different departments but frame time is not shared, so separate budgets are needed. Department is only for timing profiling not memory tracking.
// For shared memory pools across departments we need a different tracking mechanism, once Razix is big enough we can implement a full fledged memory tracking system across departments
// for now we keep it decoupled and simple and separate. These new budgets for time and memory can be created after Tanu prologue is profiled and we have a better idea of memory usage across different departments.
// TODO: Create GitHub issue to track this

namespace Razix {
    namespace Memory {

        enum RZMemoryPoolType : u8
        {
            RZ_MEM_POOL_TYPE_CORE_SYSTEMS,     // Core infrastructure (memory manager, file system, threads)
            RZ_MEM_POOL_TYPE_ASSET_POOL,       // Asset database and loaders
            RZ_MEM_POOL_TYPE_GFX_RESOURCES,    // GPU resources (textures, buffers, shaders)
            RZ_MEM_POOL_TYPE_RENDERING,        // Rendering commands and frame data
            RZ_MEM_POOL_TYPE_COUNT
        };

        enum RZMemoryPoolAllocStrategy : u8
        {
            RZ_ALLOC_STRATEGY_BUMP,
            RZ_ALLOC_STRATEGY_FREELIST,
            RZ_ALLOC_STRATEGY_RING_BUFFER,
            RZ_ALLOC_STRATEGY_COUNT
        };

        enum RZFrameAllocatorCategory : u8
        {
            RZ_FRAME_ALLOC_RENDER_THREAD,
            RZ_FRAME_ALLOC_GAME_THREAD,
            RZ_FRAME_ALLOC_WORKER_THREAD,
            RZ_FRAME_ALLOC_COUNT
        };

        typedef float DepartmentBudgetMs;
        typedef u32   ThreadFrameBudget;
        typedef float GlobalFrameBudgetMs;

        struct MemoryPoolBudget
        {
            const char* PoolName;
            u32         HeapSizeMB;
            u32         GPUMemoryMB;
        };

        struct MemPoolHasher
        {
            size_t operator()(const RZMemoryPoolType& pool) const
            {
                return rz_hash<u32>{}(static_cast<u32>(pool));
            }
        };

        // Department timing budgets loaded from config
        using DepartmentTimingBudgets = RZHashMap<Department, DepartmentBudgetMs, DepartmentHasher>;

        // Memory pool budgets
        using MemoryPoolBudgets = RZHashMap<RZMemoryPoolType, MemoryPoolBudget, MemPoolHasher>;

        // Thread budgets
        using ThreadBudgets = RZHashMap<RZString, ThreadFrameBudget>;

        RAZIX_API bool ParseBudgetFile(const RZString& filePath);

        RAZIX_API GlobalFrameBudgetMs GetGlobalFrameBudgetMs();
        RAZIX_API DepartmentBudgetMs  GetDepartmentTimingBudget(Department department);

        RAZIX_API MemoryPoolBudget  GetMemoryPoolBudget(RZMemoryPoolType poolType);
        RAZIX_API u32               GetGlobalFrameAllocatorBudget();
        RAZIX_API ThreadFrameBudget GetFrameAllocatorBudget(const RZString& threadName);

    }    // namespace Memory
}    // namespace Razix