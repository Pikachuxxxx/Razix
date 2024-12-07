// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZiniParser.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

namespace Razix {
    namespace Utilities {
        /*
        struct PrintVisitor
        {
            void operator()(int value) const
            {
                std::cout << value;
            }

            void operator()(float value) const
            {
                std::cout << value;
            }

            void operator()(bool value) const
            {
                std::cout << (value ? "true" : "false");
            }

            void operator()(const std::string& value) const
            {
                std::cout << value;
            }
        };

        // Function to print the ValueType (variant)
        void printValue(const ValueType& value)
        {
            std::visit(PrintVisitor(), value);    // Apply the visitor
        }

        // TEST HELPER FUNCTION
        static void printSection(const Section& section, int indent = 0)
        {
            // Helper to print with indentation
            auto printIndent = [indent]() {
                for (int i = 0; i < indent; ++i) {
                    std::cout << "  ";    // Two spaces per indentation level
                }
            };
            // Print variables
            for (const auto& var: section.variables) {
                printIndent();
                printValue(var.second);
            }
            // Recursively print subsections
            for (const auto& sub: section.subsections) {
                printIndent();
                std::cout << "[" << sub.first << "]" << std::endl;
                printSection(sub.second, indent + 1);    // Increase indent for subsections
            }
        }
        */

        static bool isBool(const std::string& str)
        {
            return (str == "true" || str == "false");
        }

        static ValueType decodeString(const std::string& input)
        {
            // Check if the string represents a boolean
            if (isBool(input)) {
                return input == "true";
            }

            // Check if the string represents an integer
            int                intValue;
            std::istringstream intStream(input);
            if (intStream >> intValue && intStream.eof()) {
                return intValue;
            }

            // Check if the string represents a float
            float              floatValue;
            std::istringstream floatStream(input);
            if (floatStream >> floatValue && floatStream.eof()) {
                return floatValue;
            }

            // If none of the above, store the string as is
            return input;
        }

        //----------------------------------------------------

        bool RZiniParser::parse(const std::string& filePath, bool skipVFS)
        {
            std::string physicalPath = filePath;
            if (!skipVFS) {
                if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
                    return false;
            }

            std::string              textFileStr = RZFileSystem::ReadTextFile(physicalPath);
            std::vector<std::string> lines       = Utilities::GetLines(textFileStr);

            Section     currentSection;
            std::string currentSectionName;

            for (std::string& line: lines) {
                line = Utilities::TrimWhitespaces(line);

                // Skip empty lines and comments
                if (line.empty() || line[0] == ';')
                    continue;

                if (line[0] == '[') {
                    // Section header
                    size_t endPos = line.find(']');
                    if (endPos != std::string::npos) {
                        currentSectionName = line.substr(1, endPos - 1);
                        currentSection     = m_Sections[currentSectionName];
                    }
                } else {
                    // Key-value pair
                    if (!parseKeyValue(currentSectionName, currentSection, line)) {
                        RAZIX_CORE_ERROR("Failed to parse key value pair at line: {0}", line);
                        return false;
                    }
                    // update entry
                    m_Sections[currentSectionName] = currentSection;
                }
            }

            //for (auto& kv: m_Sections) {
            //    std::cout << "Section : " << kv.first << " | ";
            //    printSection(kv.second);
            //}

            return true;
        }

        bool RZiniParser::parseKeyValue(const std::string& sectionName, Section& section, std::string& line)
        {
            line = Utilities::TrimWhitespaces(line);
            line = Utilities::RemoveSpaces(line);

            // 1. break by = to get crude key-value pair
            // 2. break key if has "." into subsection
            // 3. remove any ;xxx from value before assigning
            std::vector<std::string> KeyValue = SplitString(line, '=');
            if (KeyValue.size() > 2) {
                RAZIX_CORE_ERROR("Multiple key value pairs found");
                return false;
            }

            std::string Key                  = KeyValue[0];
            std::string ValueWithoutComments = SplitString(KeyValue[1], ';')[0];

            // check if it has subsection
            if (Utilities::StringContains(Key, ".")) {
                std::vector<std::string> KeySubKey = SplitString(Key, '.');
                if (KeySubKey.size() > 2) {
                    RAZIX_CORE_ERROR("Subsection has nested sections > 1 depth, not supported!");
                    return false;
                }
                section.subsections[KeySubKey[0]].variables[KeySubKey[1]] = decodeString(ValueWithoutComments);

            } else {
                section.variables[Key] = decodeString(ValueWithoutComments);
            }
            return true;
        }
    }    // namespace Utilities
}    // namespace Razix
