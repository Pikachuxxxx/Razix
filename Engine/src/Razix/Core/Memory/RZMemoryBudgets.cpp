// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMemoryBudgets.h"

#include "Razix/Utilities/RZiniParser.h"

namespace Razix {
    namespace Memory {

        /* The global frame budget for the entire Engine */
        static BudgetInfo s_GlobalFrameBudget;
        /* Per-Frame/Per-department budget */
        static DepartmentBudgets s_DepartmentBudgets;

        bool ParseBudgetFile(const std::string& filePath)
        {
            Utilities::RZiniParser budgetFileIniParser;
            bool                   success = budgetFileIniParser.parse(filePath);

            if (success) {
                // Get global vars
                budgetFileIniParser.getValue<float>("Departments", "FrameBudget", s_GlobalFrameBudget.CPUframeBudget);
                budgetFileIniParser.getValue<int>("Departments", "MemoryBudget", s_GlobalFrameBudget.MemoryBudget);

                // get dept budgets parse
                // start from 1 cause skip NONE
                for (auto& departInfo: s_DepartmentInfo) {
                    if (departInfo.first == Department::NONE)
                        continue;
                    std::string frameBudgetKey  = departInfo.second.debugName + std::string(".FrameBudget");
                    std::string memoryBudgetKey = departInfo.second.debugName + std::string(".MemoryBudget");

                    BudgetInfo budget;
                    if (!budgetFileIniParser.getValue<float>("Departments", frameBudgetKey, budget.CPUframeBudget))
                        RAZIX_CORE_ERROR("[Memory] failed to parse frame budget for {0}, make sure it's a float in .ini file and the value exists.", frameBudgetKey);
                    if (!budgetFileIniParser.getValue<int>("Departments", memoryBudgetKey, budget.MemoryBudget))
                        RAZIX_CORE_ERROR("[Memory] failed to parse memoroy budget for {0}, make sure it's a int in .ini file and the value exists.", memoryBudgetKey);

                    s_DepartmentBudgets[departInfo.first] = budget;
                }

            } 
            return success;
        }

        BudgetInfo GetGlobalFrameBudget()
        {
            return s_GlobalFrameBudget;
        }

        BudgetInfo GetDepartmentBudgetInfo(Department department)
        {
            return s_DepartmentBudgets[department];
        }
    }    // namespace Memory
}    // namespace Razix
