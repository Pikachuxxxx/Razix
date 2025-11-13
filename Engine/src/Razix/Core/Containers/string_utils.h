#ifndef _RZ_STRING_UTILS_H_
#define _RZ_STRING_UTILS_H_

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/string.h"

namespace Razix {

    RAZIX_API RZString GetFilePathExtension(const RZString& FileName);
    RAZIX_API RZString RemoveFilePathExtension(const RZString& FileName);
    RAZIX_API RZString GetFileName(const RZString& FilePath);
    RAZIX_API RZString RemoveName(const RZString& FilePath);
    RAZIX_API RZString GetFileLocation(const RZString& FilePath);
    RAZIX_API bool     IsHiddenFile(const RZString& path);
    RAZIX_API std::vector<RZString> SplitString(const RZString& string, const RZString& delimiters);
    RAZIX_API std::vector<RZString> SplitString(const RZString& string, const char delimiter);
    RAZIX_API std::vector<RZString> Tokenize(const RZString& string);
    RAZIX_API std::vector<RZString> GetLines(const RZString& string);
    RAZIX_API const char*              FindToken(const char* str, const RZString& token);
    RAZIX_API const char*              FindToken(const RZString& string, const RZString& token);
    RAZIX_API int32_t                  FindStringPosition(const RZString& string, const RZString& search, uint32_t offset = 0);
    RAZIX_API RZString                 StringRange(const RZString& string, uint32_t start, uint32_t length);
    RAZIX_API RZString                 RemoveStringRange(const RZString& string, uint32_t start, uint32_t length);
    RAZIX_API RZString                 TrimWhitespaces(const RZString& string);
    RAZIX_API bool                     StringContains(const RZString& string, const RZString& chars);
    RAZIX_API bool                     StartsWith(const RZString& string, const RZString& start);
    RAZIX_API int32_t                  NextInt(const RZString& string);
    RAZIX_API bool                     StringEquals(const RZString& string1, const RZString& string2);
    RAZIX_API RZString                 StringReplace(RZString str, char ch1, char ch2);
    RAZIX_API RZString                 StringReplace(RZString str, char ch);
    RAZIX_API RZString&                BackSlashesToSlashes(RZString& string);
    RAZIX_API RZString&                SlashesToBackSlashes(RZString& string);
    RAZIX_API RZString&                RemoveSpaces(RZString& string);
    RAZIX_API RZString                 Demangle(const RZString& string);

}    // namespace Razix

#endif    // _RZ_STRING_UTILS_H_
