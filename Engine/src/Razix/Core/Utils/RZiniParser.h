#pragma once

#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Containers/string_utils.h"

#include <variant>

namespace Razix {

    /**
        * INI File Format
        * ----------------
        * [Section]                                 // A section, e.g., "Global"
        *     Key = Value                           // A key-value pair, e.g., "FrameBudget = 16.67"
        *     Key.SubKey = Value                    // Key with a subsection, e.g., "Core.FrameBudget = 5"
        *     ; Comments are denoted by semicolons
        * 
        * Sections are defined by square brackets, and can contain simple key-value pairs
        * or key-value pairs with a subsection (indicated by a dot in the key name).
        * We only support parsing subsections of depth 1, please avoid nested sub-sections.
        * 
        * The value types can be:
        * - Integer (e.g., 5)
        * - Float (e.g., 16.67)
        * - Boolean (true/false)
        * - String (e.g., "Hello")
        * 
        * Note/FIXME: string value types cannot contain white spaces!
        */

    // Union to handle different types for values (string, int, bool, float)
    using ValueType = std::variant<int, float, bool, RZString, u32>;

    struct Section
    {
        RZHashMap<RZString, ValueType> variables;      // Key-value pairs
        RZHashMap<RZString, Section>   subsections;    // Subsections (nested sections)
    };

    class RZiniParser
    {
    public:
        RZiniParser()  = default;
        ~RZiniParser() = default;

        bool parse(const RZString& filePath, bool skipVFS = false);

        /**
             * This function helps find the value given the key, this also works for sub-key
             * supply sub-key as is using the "." ex. "VFX.MemoryBudget"
             */
        template<typename T>
        bool getValue(const RZString& sectionName, const RZString& key, T& value)
        {
            // check if the key has a sub-key
            if (StringContains(key, ".")) {
                RZDynamicArray<RZString> KeySubKey = SplitString(key, '.');
                if (KeySubKey.size() > 2) {
                    RAZIX_CORE_ERROR("Subsection has nested sections > 1 depth, not supported!");
                    return false;
                }
                return getValueFromSubsection<T>(sectionName, KeySubKey[0], KeySubKey[1], value);
            } else {
                // Plain key value pairs query the variables and return
                auto it = m_Sections.find(sectionName);
                if (it != m_Sections.end()) {
                    auto& variables = it->second.variables;
                    auto  varIt     = variables.find(key);
                    if (varIt != variables.end()) {
                        if (std::holds_alternative<T>(varIt->second)) {
                            value = std::get<T>(varIt->second);
                            return true;
                        } else
                            RAZIX_CORE_ERROR("[INI Parser] Trying to get type {0}, but the variable holds {1} ", typeid(value).name(), typeid(varIt->second).name());
                    }
                }
            }
            RAZIX_CORE_ERROR("[INI Parser] could not find key!");
            return false;
        }

    protected:    // Protexted only for the
        RZHashMap<RZString, Section> m_Sections;

    private:
        bool parseKeyValue(const RZString& sectionName, Section& section, RZString& line);

        template<typename T>
        bool getValueFromSubsection(const RZString& sectionName, const RZString& subSection, const RZString& key, T& value)
        {
            auto it = m_Sections.find(sectionName);
            if (it != m_Sections.end()) {
                auto& subsections = it->second.subsections;
                auto  subsecIt    = subsections.find(subSection);
                if (subsecIt != subsections.end()) {
                    auto& variables = subsecIt->second.variables;
                    auto  varIt     = variables.find(key);
                    if (varIt != variables.end()) {
                        if (std::holds_alternative<T>(varIt->second)) {
                            value = std::get<T>(varIt->second);
                            return true;
                        } else
                            RAZIX_CORE_ERROR("[INI Parser] Trying to get type {0}, but the variable holds {1} ", typeid(value).name(), typeid(varIt->second).name());
                    }
                }
            }
            RAZIX_CORE_ERROR("[INI Parser] could not find sub-key!");
            return false;
        }
    };
}    // namespace Razix
