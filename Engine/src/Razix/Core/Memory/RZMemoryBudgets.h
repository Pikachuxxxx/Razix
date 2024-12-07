#pragma once

#include "Razix/Core/RZDepartments.h"

namespace Razix {
    namespace Memory {

        struct BudgetInfo
        {
            float CPUframeBudget = 0.0f;    // Frame budget in milliseconds (CPU)
            int   MemoryBudget   = 0;       // Memory budget in MB
        };

        // This is loaded from the config file: RazixDepartmentBudgets.ini
        using DepartmentBudgets = std::unordered_map<Department, BudgetInfo>;

        /**
         * Parses the budget ini file to assign memory/frame time constraints
         */
        RAZIX_API bool ParseBudgetFile(const std::string& filePath);
        /**
         * Get the total frame budget
         */
        RAZIX_API BudgetInfo GetGlobalFrameBudget();
        /**
         * Get the per-frame budget constraints
         */
        RAZIX_API BudgetInfo GetDepartmentBudgetInfo(Department department);
    }    // namespace Memory
}    // namespace Razix