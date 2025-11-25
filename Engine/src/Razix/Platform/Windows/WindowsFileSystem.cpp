// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/OS/RZFileSystem.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <Windows.h>
    #include <fileapi.h>
    #include <wtypes.h>

namespace Razix {

    bool RZFileSystem::CreateDir(const RZString& path)
    {
        return CreateDirectoryA((LPCSTR) path.c_str(), NULL);
    }

    static HANDLE OpenFileForReading(const RZString& path)
    {
        return CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    }

    static int64_t GetFileSizeInternal(const HANDLE file)
    {
        LARGE_INTEGER size;
        GetFileSizeEx(file, &size);
        return size.QuadPart;
    }

    static bool ReadFileInternal(const HANDLE file, void* buffer, const int64_t size)
    {
        OVERLAPPED ol = {0};
    #pragma warning(push, 0)
        return ReadFileEx(file, buffer, static_cast<DWORD>(size), &ol, nullptr) != 0;
    #pragma warning(pop)
    }

    bool RZFileSystem::FileExists(const RZString& path)
    {
        auto dwAttr = GetFileAttributes((LPCSTR) path.c_str());
        return (dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    bool RZFileSystem::FolderExists(const RZString& path)
    {
        DWORD dwAttrib = GetFileAttributes(path.c_str());
        return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    int64_t RZFileSystem::GetFileSize(const RZString& path)
    {
        const HANDLE file = OpenFileForReading(path);
        if (file == INVALID_HANDLE_VALUE)
            return -1;
        int64_t result = GetFileSizeInternal(file);
        CloseHandle(file);

        return result;
    }

    bool RZFileSystem::ReadFile(const RZString& path, void* buffer, int64_t size)
    {
        const HANDLE file = OpenFileForReading(path);
        if (file == INVALID_HANDLE_VALUE)
            return false;

        if (size < 0)
            size = GetFileSizeInternal(file);

        bool result = ReadFileInternal(file, buffer, size);
        CloseHandle(file);
        return result;
    }

    u8* RZFileSystem::ReadFile(const RZString& path)
    {
        const HANDLE  file   = OpenFileForReading(path);
        const int64_t size   = GetFileSizeInternal(file);
        u8*           buffer = new u8[static_cast<u32>(size)];
        const bool    result = ReadFileInternal(file, buffer, size);
        CloseHandle(file);
        if (!result)
            delete[] buffer;
        return result ? buffer : nullptr;
    }

    RZString RZFileSystem::ReadTextFile(const RZString& path)
    {
        const HANDLE  file = OpenFileForReading(path);
        const int64_t size = GetFileSizeInternal(file);
        RZString      result(static_cast<u32>(size), 0);
        const bool    success = ReadFileInternal(file, &result[0], size);
        CloseHandle(file);

        if (success) {
            // Strip carriage returns manually
            RZString cleaned;
            for (sz i = 0; i < result.length(); ++i) {
                if (result[i] != '\r') {
                    cleaned.append(1, result[i]);
                }
            }
            return cleaned;
        }

        return RZString();
    }

    bool RZFileSystem::WriteFile(const RZString& path, u8* buffer, i64 size = -1)
    {
        bool         fileExists = FileExists(path);
        int          flags      = fileExists ? OPEN_EXISTING : CREATE_NEW;
        const HANDLE file       = CreateFile(path.c_str(), GENERIC_WRITE, NULL, nullptr, flags, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD written;
        ::WriteFile(file, buffer, static_cast<DWORD>(size), &written, nullptr);
        CloseHandle(file);
        return written != 0;
    }

    bool RZFileSystem::WriteTextFile(const RZString& path, const RZString& text)
    {
        return WriteFile(path, (u8*) &text[0], text.size());
    }
}    // namespace Razix

#endif