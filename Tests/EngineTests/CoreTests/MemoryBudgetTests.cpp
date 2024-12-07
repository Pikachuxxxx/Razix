// MemoryBudgetTests.cpp
// AI-generated unit tests for the RZMemoryBudget classes
#include "Razix/Core/RZCore.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include <gtest/gtest.h>
#include <string>

// Default Config Path for the Budget Test
static const std::string defaultConfigPath = "//RazixConfig/RazixDepartmentBudgets.ini";

namespace Razix {
    namespace Memory {
        // Fixture for Memory Budget Tests
        class RZBudgetTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                Razix::Debug::RZLog::StartUp();
                Razix::RZVirtualFileSystem::Get().StartUp();
                // Load the config file before running tests
                bool result = ParseBudgetFile(defaultConfigPath);
                ASSERT_TRUE(result) << "Failed to parse the budget config file at: " << defaultConfigPath;
            }

            void TearDown() override
            {
                // Cleanup if necessary
                Razix::RZVirtualFileSystem::Get().ShutDown();
                Razix::Debug::RZLog::Shutdown();
            }
        };

        // Test: Validate Global Frame Budget
        TEST_F(RZBudgetTests, GlobalFrameBudget)
        {
            BudgetInfo globalBudget = GetGlobalFrameBudget();
            EXPECT_GT(globalBudget.CPUframeBudget, 0.0f) << "Global CPU frame budget should be greater than 0.";
            EXPECT_GT(globalBudget.MemoryBudget, 0) << "Global memory budget should be greater than 0.";
        }

        // Test: Validate Department Budget Info
        TEST_F(RZBudgetTests, DepartmentBudgets)
        {
            // List of departments to test
            std::vector<Department> departments = {
                Department::Core,
                Department::Environment,
                Department::Character,
                Department::Lighting,
                Department::RayTracing,
                Department::VFX,
                Department::UI,
                Department::Debug,
                Department::Physics,
                Department::Scripting,
                Department::Audio,
                Department::Network,
            };

            for (auto department: departments) {
                BudgetInfo budget = GetDepartmentBudgetInfo(department);
                EXPECT_GT(budget.CPUframeBudget, 0.0f) << "CPU frame budget for department should be greater than 0.";
                EXPECT_GT(budget.MemoryBudget, 0) << "Memory budget for department should be greater than 0.";
            }
        }

        // Test: Invalid Department Budget
        TEST_F(RZBudgetTests, InvalidDepartmentBudget)
        {
            BudgetInfo budget = GetDepartmentBudgetInfo(static_cast<Department>(999));    // Invalid department
            EXPECT_EQ(budget.CPUframeBudget, 0.0f) << "Invalid department's CPU frame budget should be 0.";
            EXPECT_EQ(budget.MemoryBudget, 0) << "Invalid department's memory budget should be 0.";
        }
    }    // namespace Memory
}    // namespace Razix
