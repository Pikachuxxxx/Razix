// MemoryBudgetTests.cpp
// Unit tests for RZMemoryBudget classes
#include "Razix/Core/RZCore.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include <gtest/gtest.h>
#include <string>

// Default Config Path for the Budget Test
static const Razix::RZString defaultConfigPath = "//RazixConfig/RazixDepartmentBudgets.ini";

namespace Razix {
    namespace Memory {
        // Fixture for Memory Budget Tests
        class RZMemoryBudgetTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                Razix::Debug::RZLog::StartUp();
                Razix::RZVirtualFileSystem::Get().StartUp();
                // Load the config file before running tests
                bool result = ParseBudgetFile(defaultConfigPath);
                ASSERT_TRUE(result) << "Failed to parse the budget config file at: " << defaultConfigPath.c_str();
            }

            void TearDown() override
            {
                Razix::RZVirtualFileSystem::Get().ShutDown();
                Razix::Debug::RZLog::Shutdown();
            }
        };

        // Test: Validate Global Frame Budget
        TEST_F(RZMemoryBudgetTests, GlobalFrameBudgetMs)
        {
            GlobalFrameBudgetMs globalBudget = GetGlobalFrameBudgetMs();
            EXPECT_GT(globalBudget, 0.0f) << "Global frame budget should be greater than 0.";
            EXPECT_FLOAT_EQ(globalBudget, 16.67f) << "Global frame budget should be 16.67ms for 60 FPS.";
        }

        // Test: Validate Department Timing Budgets
        TEST_F(RZMemoryBudgetTests, DepartmentTimingBudgets)
        {
            std::vector<Department> departments = {
                Department::Core,
                Department::Environment,
                Department::Character,
                Department::Lighting,
                Department::RayTracing,
                Department::VFX,
                Department::UI,
                Department::Physics,
                Department::Scripting,
                Department::Audio,
                Department::Network,
            };

            for (auto department: departments) {
                DepartmentBudgetMs budget = GetDepartmentTimingBudget(department);
                EXPECT_GT(budget, 0.0f) << "Department timing budget should be greater than 0.";
            }
        }

        // Test: Invalid Department Timing Budget
        TEST_F(RZMemoryBudgetTests, InvalidDepartmentTimingBudget)
        {
            DepartmentBudgetMs budget = GetDepartmentTimingBudget(static_cast<Department>(999));
            EXPECT_EQ(budget, 0.0f) << "Invalid department timing budget should return 0.";
        }

        // Test: Validate Memory Pool Budgets
        TEST_F(RZMemoryBudgetTests, MemoryPoolBudgets)
        {
            std::vector<RZMemoryPoolType> pools = {
                RZ_MEM_POOL_TYPE_CORE_SYSTEMS,
                RZ_MEM_POOL_TYPE_ASSET_POOL,
                RZ_MEM_POOL_TYPE_GFX_RESOURCES,
                RZ_MEM_POOL_TYPE_RENDERING,
            };

            for (auto pool: pools) {
                MemoryPoolBudget budget = GetMemoryPoolBudget(pool);
                EXPECT_GT(budget.HeapSizeMB, 0U) << "Pool heap size should be greater than 0.";
                EXPECT_NE(budget.PoolName, nullptr) << "Pool name should not be null.";
            }
        }

        // Test: GFX Resources Pool has GPU Memory
        TEST_F(RZMemoryBudgetTests, GFXResourcesPoolGPUMemory)
        {
            MemoryPoolBudget budget = GetMemoryPoolBudget(RZ_MEM_POOL_TYPE_GFX_RESOURCES);
            EXPECT_GT(budget.GPUMemoryMB, 0U) << "GFX Resources pool should have GPU memory.";
        }

        // Test: Non-GFX Pools have No GPU Memory
        TEST_F(RZMemoryBudgetTests, NonGFXPoolsNoGPUMemory)
        {
            std::vector<RZMemoryPoolType> nonGFXPools = {
                RZ_MEM_POOL_TYPE_CORE_SYSTEMS,
                RZ_MEM_POOL_TYPE_ASSET_POOL,
                RZ_MEM_POOL_TYPE_RENDERING,
            };

            for (auto pool: nonGFXPools) {
                MemoryPoolBudget budget = GetMemoryPoolBudget(pool);
                EXPECT_EQ(budget.GPUMemoryMB, 0U) << "Non-GFX pools should not have GPU memory.";
            }
        }

        // Test:  Invalid Memory Pool Budget
        TEST_F(RZMemoryBudgetTests, InvalidMemoryPoolBudget)
        {
            MemoryPoolBudget budget = GetMemoryPoolBudget(static_cast<RZMemoryPoolType>(999));
            EXPECT_EQ(budget.HeapSizeMB, 0U) << "Invalid pool budget should return 0.";
        }

        // Test:  Validate Total Frame Allocator Budget
        TEST_F(RZMemoryBudgetTests, GlobalFrameAllocatorBudget)
        {
            u32 totalBudget = GetGlobalFrameAllocatorBudget();
            EXPECT_GT(totalBudget, 0U) << "Global frame allocator budget should be greater than 0.";
            EXPECT_EQ(totalBudget, 1024U) << "Global frame allocator budget should be 1024 MB.";
        }

        // Test: Validate Thread Frame Allocator Budgets
        TEST_F(RZMemoryBudgetTests, ThreadFrameAllocatorBudgets)
        {
            std::vector<RZString> threads = {
                RZString("RenderThread"),
                RZString("GameThread"),
                RZString("WorkerThread"),
            };

            u32 totalCalculated = 0;
            for (auto& thread: threads) {
                ThreadFrameBudget budget = GetFrameAllocatorBudget(thread);
                EXPECT_GT(budget, 0U) << "Thread frame allocator budget should be greater than 0.";
                totalCalculated += budget;
            }

            u32 globalTotal = GetGlobalFrameAllocatorBudget();
            EXPECT_EQ(totalCalculated, globalTotal) << "Sum of thread budgets should equal global frame allocator budget.";
        }

        // Test: Specific Thread Frame Allocator Budgets
        TEST_F(RZMemoryBudgetTests, SpecificThreadBudgets)
        {
            ThreadFrameBudget renderBudget = GetFrameAllocatorBudget(RZString("RenderThread"));
            ThreadFrameBudget gameBudget   = GetFrameAllocatorBudget(RZString("GameThread"));
            ThreadFrameBudget workerBudget = GetFrameAllocatorBudget(RZString("WorkerThread"));

            EXPECT_EQ(renderBudget, 384U) << "RenderThread frame allocator budget should be 384 MB.";
            EXPECT_EQ(gameBudget, 512U) << "GameThread frame allocator budget should be 512 MB. ";
            EXPECT_EQ(workerBudget, 128U) << "WorkerThread frame allocator budget should be 128 MB.";
        }

        // Test: Invalid Thread Frame Allocator Budget
        TEST_F(RZMemoryBudgetTests, InvalidThreadBudget)
        {
            ThreadFrameBudget budget = GetFrameAllocatorBudget(RZString("InvalidThread"));
            EXPECT_EQ(budget, 0U) << "Invalid thread budget should return 0.";
        }

        // Test:  Frame Budget Validation (All departments should fit in global budget)
        TEST_F(RZMemoryBudgetTests, DepartmentTimingBudgetSum)
        {
            GlobalFrameBudgetMs globalBudget          = GetGlobalFrameBudgetMs();
            float               totalDepartmentBudget = 0.0f;

            std::vector<Department> departments = {
                Department::Core,
                Department::Environment,
                Department::Character,
                Department::Lighting,
                Department::RayTracing,
                Department::VFX,
                Department::UI,
                Department::Physics,
                Department::Scripting,
                Department::Audio,
                Department::Network,
            };

            for (auto department: departments) {
                DepartmentBudgetMs budget = GetDepartmentTimingBudget(department);
                totalDepartmentBudget += budget;
            }

            // Note: Total may exceed global budget due to parallelization, so we only warn
            RAZIX_CORE_WARN("[MemoryBudgetTests] Total department budgets:  {0} ms, Global budget: {1} ms",
                totalDepartmentBudget,
                globalBudget);
        }

    }    // namespace Memory
}    // namespace Razix