// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZiniParser.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

namespace Razix {

    static bool isBool(const RZString& str)
    {
        return (str == RZString("true") || str == RZString("false"));
    }

    static bool tryParseInt(const RZString& str, int& outValue)
    {
        // Check if string is empty
        if (str.empty())
            return false;

        sz pos = 0;

        // Handle negative numbers
        if (str[0] == '-' || str[0] == '+') {
            pos = 1;
        }

        // Check if all remaining characters are digits
        for (sz i = pos; i < str.length(); ++i) {
            if (!isdigit(str[i]))
                return false;
        }

        // If we got here, it's a valid integer
        outValue = atoi(str.c_str());
        return true;
    }

    static bool tryParseFloat(const RZString& str, float& outValue)
    {
        // Check if string is empty
        if (str.empty())
            return false;

        sz   pos        = 0;
        bool hasDecimal = false;

        // Handle negative/positive sign
        if (str[0] == '-' || str[0] == '+') {
            pos = 1;
        }

        // Check if all characters are digits or a single decimal point
        for (sz i = pos; i < str.length(); ++i) {
            if (str[i] == '.') {
                if (hasDecimal)
                    return false;    // Multiple decimal points
                hasDecimal = true;
            } else if (!isdigit(str[i])) {
                return false;
            }
        }

        // Need at least one digit
        if (str.length() <= pos)
            return false;

        // If we got here, it's a valid float
        outValue = static_cast<float>(atof(str.c_str()));
        return true;
    }

    static ValueType decodeString(const RZString& input)
    {
        // Check if the string represents a boolean
        if (isBool(input)) {
            return input == RZString("true");
        }

        // Check if the string represents an integer
        int intValue;
        if (tryParseInt(input, intValue)) {
            return intValue;
        }

        // Check if the string represents a float
        float floatValue;
        if (tryParseFloat(input, floatValue)) {
            return floatValue;
        }

        // If none of the above, store the string as is
        return input;
    }

    //----------------------------------------------------

    bool RZiniParser::parse(const RZString& filePath, bool skipVFS)
    {
        RZString physicalPath = filePath;
        if (!skipVFS) {
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
                return false;
        }

        RZString              textFileStr = RZFileSystem::ReadTextFile(physicalPath);
        std::vector<RZString> lines       = GetLines(textFileStr);

        Section  currentSection;
        RZString currentSectionName;

        for (RZString& line: lines) {
            line = TrimWhitespaces(line);

            // Skip empty lines and comments
            if (line.empty() || line[0] == ';')
                continue;

            if (line[0] == '[') {
                // Section header
                sz endPos = line.find(']');
                if (endPos != RZString::npos) {
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

        return true;
    }

    bool RZiniParser::parseKeyValue(const RZString& sectionName, Section& section, RZString& line)
    {
        line = TrimWhitespaces(line);
        line = RemoveSpaces(line);

        // 1. break by = to get crude key-value pair
        // 2. break key if has "." into subsection
        // 3. remove any ;xxx from value before assigning
        std::vector<RZString> KeyValue = SplitString(line, '=');
        if (KeyValue.size() > 2) {
            RAZIX_CORE_ERROR("Multiple key value pairs found");
            return false;
        }

        RZString Key                  = KeyValue[0];
        RZString ValueWithoutComments = SplitString(KeyValue[1], ';')[0];

        // check if it has subsection
        if (StringContains(Key, RZString("."))) {
            std::vector<RZString> KeySubKey = SplitString(Key, '.');
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
}    // namespace Razix