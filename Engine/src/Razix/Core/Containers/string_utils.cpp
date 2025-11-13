// clang-format off
#include "rzxpch.h"
// clang-format on
#include "string_utils.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <DbgHelp.h>
    #include <windows.h>
#else
    #include <cxxabi.h>    // __cxa_demangle()
#endif

namespace Razix {

    //-----------------------------------------------------------------------------
    // STRING UTILITIES
    //-----------------------------------------------------------------------------

    RZString GetFilePathExtension(const RZString& FileName)
    {
        sz pos = FileName.find_last_of('.');
        if (pos != RZString::npos)
            return FileName.substr(pos + 1);
        return RZString();
    }

    RZString RemoveFilePathExtension(const RZString& FileName)
    {
        sz pos = FileName.find_last_of('.');
        if (pos != RZString::npos)
            return FileName.substr(0, pos);
        return FileName;
    }

    RZString GetFileName(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(pos + 1);
        return FilePath;
    }

    RZString GetFileLocation(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(0, pos + 1);
        return FilePath;
    }

    RZString RemoveName(const RZString& FilePath)
    {
        sz pos = FilePath.find_last_of('/');
        if (pos != RZString::npos)
            return FilePath.substr(0, pos + 1);
        return FilePath;
    }

    bool IsHiddenFile(const RZString& path)
    {
        if (path != RZString("..") && path != RZString(".") && path[0] == '.') {
            return true;
        }

        return false;
    }

    std::vector<RZString> SplitString(const RZString& string, const RZString& delimiters)
    {
        sz start = 0;
        sz end   = string.find_first_of(delimiters);

        std::vector<RZString> result;

        while (end <= RZString::npos) {
            RZString token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == RZString::npos)
                break;

            start = end + 1;
            end   = string.find_first_of(delimiters, start);
        }

        return result;
    }

    std::vector<RZString> SplitString(const RZString& string, char delimiter)
    {
        return SplitString(string, RZString(1, delimiter));
    }

    std::vector<RZString> Tokenize(const RZString& string)
    {
        return SplitString(string, RZString(" \t\n"));
    }

    std::vector<RZString> GetLines(const RZString& string)
    {
        return SplitString(string, RZString("\n"));
    }

    cstr FindToken(cstr str, const RZString& token)
    {
        RAZIX_ASSERT(str != NULL, "FindToken() str is null");

        RZString haystack(str);
        sz       pos = 0;

        while (pos < haystack.length()) {
            pos = haystack.find(token, pos);
            if (pos == RZString::npos)
                return nullptr;

            // Check left boundary
            bool left = (pos == 0) || (pos > 0 && isspace(haystack[pos - 1]));

            // Check right boundary
            sz   end_pos = pos + token.length();
            bool right   = (end_pos >= haystack.length()) || (end_pos < haystack.length() && isspace(haystack[end_pos]));

            if (left && right)
                return haystack.c_str() + pos;

            pos++;
        }

        return nullptr;
    }

    cstr FindToken(const RZString& string, const RZString& token)
    {
        return FindToken(string.c_str(), token);
    }

    i32 FindStringPosition(const RZString& string, const RZString& search, u32 offset)
    {
        RAZIX_ASSERT(offset <= string.length(), "FindStringPosition() offset out of range");

        sz pos = string.find(search, offset);
        if (pos == RZString::npos)
            return -1;
        return static_cast<i32>(pos);
    }

    RZString StringRange(const RZString& string, u32 start, u32 length)
    {
        return string.substr(start, length);
    }

    RZString RemoveStringRange(const RZString& string, u32 start, u32 length)
    {
        RZString result = string;
        return result.erase(start, length);
    }

    RZString TrimWhitespaces(const RZString& string)
    {
        sz start = string.find_first_not_of(RZString(" \t\n\r\f\v"));
        if (start == RZString::npos)
            return RZString();

        sz end = string.find_last_not_of(RZString(" \t\n\r\f\v"));

        return string.substr(start, end - start + 1);
    }

    bool StringContains(const RZString& string, const RZString& chars)
    {
        return string.find(chars) != RZString::npos;
    }

    bool StartsWith(const RZString& string, const RZString& start)
    {
        return string.find(start) == 0;
    }

    i32 NextInt(const RZString& string)
    {
        for (sz i = 0; i < string.size(); i++) {
            if (isdigit(string[i])) {
                // Extract the number starting from position i
                RZString numStr;
                while (i < string.size() && (isdigit(string[i]) || (numStr.empty() && string[i] == '-'))) {
                    numStr.append(1, string[i]);
                    i++;
                }
                return atoi(numStr.c_str());
            }
        }
        return -1;
    }

    bool StringEquals(const RZString& string1, const RZString& string2)
    {
        return string1 == string2;
    }

    RZString StringReplace(RZString str, char ch1, char ch2)
    {
        for (sz i = 0; i < str.length(); ++i) {
            if (str[i] == ch1)
                str[i] = ch2;
        }
        return str;
    }

    RZString StringReplace(RZString str, char ch)
    {
        for (sz i = 0; i < str.length(); ++i) {
            if (str[i] == ch) {
                str = str.substr(0, i) + str.substr(i + 1, str.length());
            }
        }
        return str;
    }

    RZString& BackSlashesToSlashes(RZString& string)
    {
        for (sz i = 0; i < string.length(); i++) {
            if (string[i] == '\\') {
                string[i] = '/';
            }
        }
        return string;
    }

    RZString& SlashesToBackSlashes(RZString& string)
    {
        for (sz i = 0; i < string.length(); i++) {
            if (string[i] == '/') {
                string[i] = '\\';
            }
        }
        return string;
    }

    RZString& RemoveSpaces(RZString& string)
    {
        sz i = 0;
        while (i < string.length()) {
            if (string[i] == ' ') {
                string = string.substr(0, i) + string.substr(i + 1, string.length());
            } else {
                i++;
            }
        }
        return string;
    }

    RZString Demangle(const RZString& string)
    {
        if (string.empty())
            return RZString();

#if defined(RAZIX_PLATFORM_WINDOWS)
        char undecorated_name[1024];
        if (!UnDecorateSymbolName(
                string.c_str(), undecorated_name, sizeof(undecorated_name), UNDNAME_COMPLETE)) {
            return string;
        } else {
            return RZString(undecorated_name);
        }
#else
        char* demangled = nullptr;
        int   status    = -1;
        demangled       = abi::__cxa_demangle(string.c_str(), nullptr, nullptr, &status);
        RZString ret    = status == 0 ? RZString(demangled) : string;
        free(demangled);
        return ret;
#endif
    }

}    // namespace Razix
