// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMemoryBudgets.h"

#include "Razix/Core/Containers/string_utils.h"
#include "Razix/Core/Utils/RZiniParser.h"

namespace Razix {
    namespace Memory {

        static GlobalFrameBudgetMs     s_GlobalFrameBudget;
        static DepartmentTimingBudgets s_DepartmentTimingBudgets;
        static MemoryPoolBudgets       s_MemoryPoolBudgets;
        static ThreadBudgets           s_ThreadBudgets;
        static u32                     s_TotalFrameAllocatorBudget = 0;

        //--------------------------------------------------------------------

        bool ParseBudgetFile(const RZString& filePath)
        {
            RZiniParser budgetFileIniParser;
            bool        success = budgetFileIniParser.parse(filePath);

            if (!success) {
                RAZIX_CORE_ERROR("[Memory] Failed to parse budget file: {0}", filePath.c_str());
                return false;
            }

            //--------------------------------------------------------------------
            // [GlobalFrameBudget]

            budgetFileIniParser.getValue<float>("GlobalFrameBudget", "TotalFrameBudgetMs", s_GlobalFrameBudget);

            //--------------------------------------------------------------------
            // [DeptTimingBudgets]

            for (auto& departInfo: s_DepartmentInfo) {
                // skip NONE and Debug departments and count, YES DEBUG!
                if (departInfo.first == Department::NONE || departInfo.first == Department::Debug || departInfo.first == Department::COUNT)
                    continue;

                RZString           timingBudgetKey = departInfo.second.debugName + RZString(".FrameBudgetMs");
                DepartmentBudgetMs timingBudget    = 0.0f;

                bool parseSuccess = budgetFileIniParser.getValue<float>("DeptTimingBudgets", timingBudgetKey, timingBudget);
                RAZIX_CORE_ASSERT(parseSuccess, "[Memory] Failed to parse timing budget for {}.   Ensure it exists in [DeptTimingBudgets] section.", timingBudgetKey.c_str());

                s_DepartmentTimingBudgets[departInfo.first] = timingBudget;
            }

            //--------------------------------------------------------------------
            // [MemoryPools]

            const char* poolNames[] = {"CoreSystems", "AssetPool", "GfxResources", "RenderingPool"};

            for (u32 i = 0; i < RZ_MEM_POOL_TYPE_COUNT; ++i) {
                RZString heapKey = RZString(poolNames[i]) + ".HeapSizeMB";
                RZString gpuKey  = RZString(poolNames[i]) + ".GPUMemoryMB";

                MemoryPoolBudget poolBudget = {};
                poolBudget.PoolName         = poolNames[i];
                poolBudget.GPUMemoryMB      = 0;

                int  heapSizeMB       = 0;
                bool heapParseSuccess = budgetFileIniParser.getValue<int>("MemoryPools", heapKey, heapSizeMB);
                RAZIX_CORE_ASSERT(heapParseSuccess, "[Memory] Failed to parse heap size for pool {}.", poolNames[i]);
                poolBudget.HeapSizeMB = static_cast<u32>(heapSizeMB);

                // GPU memory is optional, only for GfxResources
                if (i == RZ_MEM_POOL_TYPE_GFX_RESOURCES) {
                    int gpuSizeMB = 0;
                    budgetFileIniParser.getValue<int>("MemoryPools", gpuKey, gpuSizeMB);
                    poolBudget.GPUMemoryMB = static_cast<u32>(gpuSizeMB);
                }

                s_MemoryPoolBudgets[(RZMemoryPoolType) i] = poolBudget;
            }

            //--------------------------------------------------------------------
            // [ThreadFrameBudgets]

            const char* threadNames[]         = {"RenderThread", "GameThread", "WorkerThread"};
            u32         calculatedTotalBudget = 0;

            for (u32 i = 0; i < RZ_FRAME_ALLOC_COUNT; ++i) {
                RZString budgetKey = RZString(threadNames[i]) + ".PerFrameAllocatorMB";

                int  threadBudgetTemp = 0;
                bool parseSuccess     = budgetFileIniParser.getValue<int>("ThreadFrameBudgets", budgetKey, threadBudgetTemp);
                RAZIX_CORE_ASSERT(parseSuccess, "[Memory] Failed to parse thread frame budget for {}.", threadNames[i]);

                ThreadFrameBudget threadBudget            = static_cast<u32>(threadBudgetTemp);
                s_ThreadBudgets[RZString(threadNames[i])] = threadBudget;
                calculatedTotalBudget += threadBudget;
            }

            // Get the total budget and validate
            int totalFrameAllocBudgetTemp = 0;
            budgetFileIniParser.getValue<int>("ThreadFrameBudgets", "TotalFrameAllocatorBudget", totalFrameAllocBudgetTemp);
            s_TotalFrameAllocatorBudget = static_cast<u32>(totalFrameAllocBudgetTemp);

            //--------------------------------------------------------------------

            RAZIX_CORE_ASSERT(s_TotalFrameAllocatorBudget == calculatedTotalBudget,
                "[Memory] TotalFrameAllocatorBudget ({} MB) does not match sum of thread budgets ({} MB). "
                "RenderThread ({} MB) + GameThread ({} MB) + WorkerThread ({} MB)",
                s_TotalFrameAllocatorBudget,
                calculatedTotalBudget,
                s_ThreadBudgets[RZString("RenderThread")],
                s_ThreadBudgets[RZString("GameThread")],
                s_ThreadBudgets[RZString("WorkerThread")]);

            RAZIX_CORE_INFO("[Memory] Successfully parsed budget file: {0}", filePath.c_str());

            RAZIX_CORE_INFO("[Memory] Global Frame Budget: {0} ms", s_GlobalFrameBudget);
            for (auto& deptBudget: s_DepartmentTimingBudgets) {
                RAZIX_CORE_INFO("[Memory] Department {0} Budget: {1} ms",
                    s_DepartmentInfo.at(deptBudget.first).debugName,
                    deptBudget.second);
            }

            for (auto& poolBudget: s_MemoryPoolBudgets) {
                RAZIX_CORE_INFO("[Memory] Memory Pool {0} Budget: Heap: {1} MB, GPU: {2} MB",
                    poolBudget.second.PoolName,
                    poolBudget.second.HeapSizeMB,
                    poolBudget.second.GPUMemoryMB);
            }

            RAZIX_CORE_INFO("[Memory] Total Frame Allocator Budget: {0} MB (RenderThread: {1} MB, GameThread: {2} MB, WorkerThread: {3} MB)",
                s_TotalFrameAllocatorBudget,
                s_ThreadBudgets[RZString("RenderThread")],
                s_ThreadBudgets[RZString("GameThread")],
                s_ThreadBudgets[RZString("WorkerThread")]);

            return true;
        }

        //--------------------------------------------------------------------

        GlobalFrameBudgetMs GetGlobalFrameBudgetMs()
        {
            return s_GlobalFrameBudget;
        }

        DepartmentBudgetMs GetDepartmentTimingBudget(Department department)
        {
            auto it = s_DepartmentTimingBudgets.find(department);
            if (it != s_DepartmentTimingBudgets.end()) {
                return it->second;
            }
            RAZIX_CORE_WARN("[Memory] Department timing budget not found for department: {0}", (u32) department);
            return 0.0f;
        }

        MemoryPoolBudget GetMemoryPoolBudget(RZMemoryPoolType poolType)
        {
            auto it = s_MemoryPoolBudgets.find(poolType);
            if (it != s_MemoryPoolBudgets.end()) {
                return it->second;
            }
            RAZIX_CORE_WARN("[Memory] Memory pool budget not found for pool type: {0}", (u32) poolType);
            return {};
        }

        u32 GetGlobalFrameAllocatorBudget()
        {
            return s_TotalFrameAllocatorBudget;
        }

        ThreadFrameBudget GetFrameAllocatorBudget(const RZString& threadName)
        {
            auto it = s_ThreadBudgets.find(threadName);
            if (it != s_ThreadBudgets.end()) {
                return it->second;
            }
            RAZIX_CORE_WARN("[Memory] Frame allocator budget not found for thread: {0}", threadName.c_str());
            return 0;
        }

    }    // namespace Memory
}    // namespace Razix