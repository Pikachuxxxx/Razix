#pragma once

namespace Razix {
    namespace Utilities {
        template<typename T>
        static std::string ToString(const T& input)
        {
            return std::to_string(input);
        }

        RAZIX_API std::string GetFilePathExtension(const std::string& FileName);
        RAZIX_API std::string RemoveFilePathExtension(const std::string& FileName);
        RAZIX_API std::string GetFileName(const std::string& FilePath);
        RAZIX_API std::string RemoveName(const std::string& FilePath);
        RAZIX_API std::string GetFileLocation(const std::string& FilePath);
        RAZIX_API bool        IsHiddenFile(const std::string& path);
        RAZIX_API std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
        RAZIX_API std::vector<std::string> SplitString(const std::string& string, const char delimiter);
        RAZIX_API std::vector<std::string> Tokenize(const std::string& string);
        RAZIX_API std::vector<std::string> GetLines(const std::string& string);
        RAZIX_API const char*              FindToken(const char* str, const std::string& token);
        RAZIX_API const char*              FindToken(const std::string& string, const std::string& token);
        RAZIX_API int32_t                  FindStringPosition(const std::string& string, const std::string& search, uint32_t offset = 0);
        RAZIX_API std::string StringRange(const std::string& string, uint32_t start, uint32_t length);
        RAZIX_API std::string RemoveStringRange(const std::string& string, uint32_t start, uint32_t length);
        RAZIX_API std::string RemoveTabs(const std::string& string);
        RAZIX_API std::string GetBlock(const char* str, const char** outPosition = nullptr);
        RAZIX_API std::string GetBlock(const std::string& string, uint32_t offset = 0);
        RAZIX_API std::string GetStatement(const char* str, const char** outPosition = nullptr);
        RAZIX_API bool        StringContains(const std::string& string, const std::string& chars);
        RAZIX_API bool        StartsWith(const std::string& string, const std::string& start);
        RAZIX_API int32_t     NextInt(const std::string& string);
        RAZIX_API bool        StringEquals(const std::string& string1, const std::string& string2);
        RAZIX_API std::string StringReplace(std::string str, char ch1, char ch2);
        RAZIX_API std::string StringReplace(std::string str, char ch);
        RAZIX_API std::string& BackSlashesToSlashes(std::string& string);
        RAZIX_API std::string& SlashesToBackSlashes(std::string& string);
        RAZIX_API std::string& RemoveSpaces(std::string& string);
        RAZIX_API std::string Demangle(const std::string& string);
    }    // namespace Utilities
}    // namespace Razix
