#pragma once

#include "Razix/Core/Utils/RZStringUtilities.h"

namespace Razix {
    namespace Utilities {

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
        using ValueType = std::variant<int, float, bool, std::string, u32>;

        struct Section
        {
            std::unordered_map<std::string, ValueType> variables;      // Key-value pairs
            std::unordered_map<std::string, Section>   subsections;    // Subsections (nested sections)
        };

        class RZiniParser
        {
        public:
            RZiniParser()  = default;
            ~RZiniParser() = default;

            bool parse(const std::string& filePath, bool skipVFS = false);

            /**
             * This function helps find the value given the key, this also works for sub-key
             * supply sub-key as is using the "." ex. "VFX.MemoryBudget"
             */
            template<typename T>
            bool getValue(const std::string& sectionName, const std::string& key, T& value)
            {
                // check if the key has a sub-key
                if (Utilities::StringContains(key, ".")) {
                    std::vector<std::string> KeySubKey = SplitString(key, '.');
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
            std::unordered_map<std::string, Section> m_Sections;

        private:
            bool parseKeyValue(const std::string& sectionName, Section& section, std::string& line);

            template<typename T>
            bool getValueFromSubsection(const std::string& sectionName, const std::string& subSection, const std::string& key, T& value)
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
    }    // namespace Utilities
}    // namespace Razix
