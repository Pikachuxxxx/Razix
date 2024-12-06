// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZStringUtilities.h"

#include <cctype>

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <DbgHelp.h>
    #include <windows.h>
#else
    #include <cxxabi.h>    // __cxa_demangle()
#endif

namespace Razix {
    namespace Utilities {
        std::string GetFilePathExtension(const std::string& FileName)
        {
            auto pos = FileName.find_last_of('.');
            if (pos != std::string::npos)
                return FileName.substr(pos + 1);
            return "";
        }

        std::string RemoveFilePathExtension(const std::string& FileName)
        {
            auto pos = FileName.find_last_of('.');
            if (pos != std::string::npos)
                return FileName.substr(0, pos);
            return FileName;
        }

        std::string GetFileName(const std::string& FilePath)
        {
            auto pos = FilePath.find_last_of('/');
            if (pos != std::string::npos)
                return FilePath.substr(pos + 1);
            return FilePath;
        }

        std::string GetFileLocation(const std::string& FilePath)
        {
            auto pos = FilePath.find_last_of('/');
            if (pos != std::string::npos)
                return FilePath.substr(0, pos + 1);
            return FilePath;
        }

        std::string RemoveName(const std::string& FilePath)
        {
            auto pos = FilePath.find_last_of('/');
            if (pos != std::string::npos)
                return FilePath.substr(0, pos + 1);
            return FilePath;
        }

        bool IsHiddenFile(const std::string& path)
        {
            if (path != ".." && path != "." && path[0] == '.') {
                return true;
            }

            return false;
        }

        std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
        {
            sz start = 0;
            sz end   = string.find_first_of(delimiters);

            std::vector<std::string> result;

            while (end <= std::string::npos) {
                std::string token = string.substr(start, end - start);
                if (!token.empty())
                    result.push_back(token);

                if (end == std::string::npos)
                    break;

                start = end + 1;
                end   = string.find_first_of(delimiters, start);
            }

            return result;
        }

        std::vector<std::string> SplitString(const std::string& string, const char delimiter)
        {
            return SplitString(string, std::string(1, delimiter));
        }

        std::vector<std::string> Tokenize(const std::string& string)
        {
            return SplitString(string, " \t\n");
        }

        std::vector<std::string> GetLines(const std::string& string)
        {
            return SplitString(string, "\n");
        }

        cstr FindToken(cstr str, const std::string& token)
        {
            cstr t = str;
            while ((t = strstr(t, token.c_str()))) {
                bool left  = str == t || isspace(t[-1]);
                bool right = !t[token.size()] || isspace(t[token.size()]);
                if (left && right)
                    return t;

                t += token.size();
            }
            return nullptr;
        }

        cstr FindToken(const std::string& string, const std::string& token)
        {
            return FindToken(string.c_str(), token);
        }

        int32_t FindStringPosition(const std::string& string, const std::string& search, u32 offset)
        {
            cstr str   = string.c_str() + offset;
            cstr found = strstr(str, search.c_str());
            if (found == nullptr)
                return -1;
            return (int32_t) (found - str) + offset;
        }

        std::string StringRange(const std::string& string, u32 start, u32 length)
        {
            return string.substr(start, length);
        }

        std::string RemoveStringRange(const std::string& string, u32 start, u32 length)
        {
            std::string result = string;
            return result.erase(start, length);
        }

        std::string RemoveTabs(const std::string& string)
        {
            std::string result = string;
            return result.erase(0, string.find_first_not_of(" \t\n\r\f\v"));
        }

        std::string GetBlock(cstr str, cstr* outPosition)
        {
            cstr end = strstr(str, "}");
            if (!end)
                return std::string(str);

            if (outPosition)
                *outPosition = end;
            const u32 length = static_cast<u32>(end - str + 1);
            return std::string(str, length);
        }

        std::string GetBlock(const std::string& string, u32 offset)
        {
            cstr str = string.c_str() + offset;
            return Utilities::GetBlock(str);
        }

        std::string GetStatement(cstr str, cstr* outPosition)
        {
            cstr end = strstr(str, ";");
            if (!end)
                return std::string(str);

            if (outPosition)
                *outPosition = end;
            const u32 length = static_cast<u32>(end - str + 1);
            return std::string(str, length);
        }

        bool StringContains(const std::string& string, const std::string& chars)
        {
            return string.find(chars) != std::string::npos;
        }

        bool StartsWith(const std::string& string, const std::string& start)
        {
            return string.find(start) == 0;
        }

        int32_t NextInt(const std::string& string)
        {
            for (u32 i = 0; i < string.size(); i++) {
                if (isdigit(string[i]))
                    return atoi(&string[i]);
            }
            return -1;
        }

        bool StringEquals(const std::string& string1, const std::string& string2)
        {
            return strcmp(string1.c_str(), string2.c_str()) == 0;
        }

        std::string StringReplace(std::string str, char ch1, char ch2)
        {
            for (int i = 0; i < str.length(); ++i) {
                if (str[i] == ch1)
                    str[i] = ch2;
            }

            return str;
        }

        std::string StringReplace(std::string str, char ch)
        {
            for (int i = 0; i < str.length(); ++i) {
                if (str[i] == ch) {
                    str = std::string(str).substr(0, i) + std::string(str).substr(i + 1, str.length());
                }
            }

            return str;
        }

        std::string& BackSlashesToSlashes(std::string& string)
        {
            sz len = string.length();
            for (sz i = 0; i < len; i++) {
                if (string[i] == '\\') {
                    string[i] = '/';
                }
            }
            return string;
        }

        std::string& SlashesToBackSlashes(std::string& string)
        {
            sz len = string.length();
            for (sz i = 0; i < len; i++) {
                if (string[i] == '/') {
                    string[i] = '\\';
                }
            }
            return string;
        }

        std::string& RemoveSpaces(std::string& string)
        {
            std::string::iterator endIterator = std::remove(string.begin(), string.end(), ' ');
            string.erase(endIterator, string.end());
            string.erase(std::remove_if(string.begin(),
                             string.end(),
                             [](unsigned char x) {
                                 return std::isspace(x);
                             }),
                string.end());

            return string;
        }

        std::string Demangle(const std::string& string)
        {
            if (string.empty())
                return {};

#if defined(RAZIX_PLATFORM_WINDOWS)
            char undecorated_name[1024];
            if (!UnDecorateSymbolName(
                    string.c_str(), undecorated_name, sizeof(undecorated_name), UNDNAME_COMPLETE)) {
                return string;
            } else {
                return std::string(undecorated_name);
            }
#else
            char* demangled = nullptr;
            int   status    = -1;
            demangled       = abi::__cxa_demangle(string.c_str(), nullptr, nullptr, &status);
            std::string ret = status == 0 ? std::string(demangled) : string;
            free(demangled);
            return ret;
#endif
        }
    }    // namespace Utilities
}    // namespace Razix
