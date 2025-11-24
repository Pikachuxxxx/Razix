// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMemoryBudgets.h"

#include "Razix/Core/Containers/string_utils.h"
#include "Razix/Core/Utils/RZiniParser.h"

namespace Razix {
    namespace Memory {

        /* The global frame budget for the entire Engine */
        static BudgetInfo s_GlobalFrameBudget;
        /* Per-Frame/Per-department budget */
        static DepartmentBudgets s_DepartmentBudgets;

        bool ParseBudgetFile(const RZString& filePath)
        {
            RZiniParser budgetFileIniParser;
            bool        success = budgetFileIniParser.parse(filePath);

            if (success) {
                // Get global vars
                budgetFileIniParser.getValue<float>("Departments", "FrameBudget", s_GlobalFrameBudget.CPUframeBudget);
                budgetFileIniParser.getValue<int>("Departments", "MemoryBudget", s_GlobalFrameBudget.MemoryBudget);

                // get dept budgets parse
                // start from 1 cause skip NONE
                for (auto& departInfo: s_DepartmentInfo) {
                    if (departInfo.first == Department::NONE)
                        continue;
                    RZString frameBudgetKey  = departInfo.second.debugName + RZString(".FrameBudget");
                    RZString memoryBudgetKey = departInfo.second.debugName + RZString(".MemoryBudget");

                    BudgetInfo budget                  = {};
                    bool       frameBudgetParseSuccess = budgetFileIniParser.getValue<float>("Departments", frameBudgetKey, budget.CPUframeBudget);
                    RAZIX_UNUSED(frameBudgetParseSuccess);
                    RAZIX_CORE_ASSERT(frameBudgetParseSuccess, "[Memory] failed to parse frame budget for {0}, make sure it's a float in .ini file and the value exists.", frameBudgetKey.c_str());
                    bool memoryBudgetParseSuccess = budgetFileIniParser.getValue<int>("Departments", memoryBudgetKey, budget.MemoryBudget);
                    RAZIX_UNUSED(memoryBudgetParseSuccess);
                    RAZIX_CORE_ASSERT(memoryBudgetParseSuccess, "[Memory] failed to parse memoroy budget for {0}, make sure it's a int in .ini file and the value exists.", memoryBudgetKey.c_str());

                    RAZIX_UNUSED(frameBudgetParseSuccess);
                    RAZIX_UNUSED(memoryBudgetParseSuccess);

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
