// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/OS/RZFileSystem.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <Windows.h>
    #include <fileapi.h>
    #include <string>
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

    //--------------------------------------------------------------------------
    // File Watcher
    //--------------------------------------------------------------------------

    struct WindowsFileWatcherState
    {
        HANDLE       hDir;
        bool         watchFile;           // true = single-file mode
        RZString     rootPath;            // watched directory path
        RZString     filterFileName;      // non-empty when watchFile == true
        DWORD        notifyFilter;
        BYTE         buffer[8192];
        OVERLAPPED   overlapped;
        bool         pendingIo;
    };

    static void WindowsFileWatcherPoll(RZFileWatcher* watcher, RZFileChange* outChanges, int* inOutCount, int maxChanges)
    {
        if (!watcher || !watcher->platform || !outChanges || !inOutCount)
            return;

        auto* state = static_cast<WindowsFileWatcherState*>(watcher->platform);
        *inOutCount  = 0;

        // Issue or re-issue ReadDirectoryChangesW if not already pending
        if (!state->pendingIo) {
            BOOL ok = ::ReadDirectoryChangesW(
                state->hDir,
                state->buffer,
                sizeof(state->buffer),
                FALSE,
                state->notifyFilter,
                NULL,
                &state->overlapped,
                NULL);
            if (ok)
                state->pendingIo = true;
            else
                return;
        }

        // Non-blocking: check if IO has completed
        DWORD bytesTransferred = 0;
        BOOL  result           = ::GetOverlappedResult(state->hDir, &state->overlapped, &bytesTransferred, FALSE);
        if (!result) {
            // IO still pending or error – nothing to report yet
            return;
        }

        state->pendingIo = false;

        if (bytesTransferred == 0)
            return;

        const BYTE* ptr = state->buffer;
        while (*inOutCount < maxChanges) {
            const auto* info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(ptr);

            // Convert wide filename to narrow
            int len = WideCharToMultiByte(CP_UTF8, 0, info->FileName, (int) (info->FileNameLength / sizeof(WCHAR)), NULL, 0, NULL, NULL);
            if (len > 0) {
                std::string name(len, '\0');
                WideCharToMultiByte(CP_UTF8, 0, info->FileName, (int) (info->FileNameLength / sizeof(WCHAR)), &name[0], len, NULL, NULL);

                // In single-file mode, only process entries that match our target file
                bool shouldProcess = !state->watchFile || state->filterFileName.empty() || (state->filterFileName == name.c_str());
                if (shouldProcess) {
                    RZFileChange& change = outChanges[*inOutCount];
                    change.path          = (state->rootPath + "/" + name.c_str()).c_str();

                    switch (info->Action) {
                        case FILE_ACTION_MODIFIED:
                            change.type = RZFileChangeType::Modified;
                            ++(*inOutCount);
                            break;
                        case FILE_ACTION_ADDED:
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            change.type = RZFileChangeType::Added;
                            ++(*inOutCount);
                            break;
                        case FILE_ACTION_REMOVED:
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            change.type = RZFileChangeType::Removed;
                            ++(*inOutCount);
                            break;
                        default:
                            break;
                    }
                }
            }

            if (info->NextEntryOffset == 0)
                break;
            ptr += info->NextEntryOffset;
        }

        // Reset overlapped for next call
        ZeroMemory(&state->overlapped, sizeof(state->overlapped));
    }

    static void WindowsFileWatcherDestroy(RZFileWatcher* watcher)
    {
        if (!watcher || !watcher->platform)
            return;

        auto* state = static_cast<WindowsFileWatcherState*>(watcher->platform);
        if (state->hDir != INVALID_HANDLE_VALUE)
            CloseHandle(state->hDir);

        delete state;
        delete watcher;
    }

    static RZFileWatcher* CreateWindowsWatcher(const RZString& dirPath, bool watchFile, const RZString& fileName)
    {
        HANDLE hDir = CreateFileA(
            dirPath.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL);

        if (hDir == INVALID_HANDLE_VALUE)
            return nullptr;

        auto* state              = new WindowsFileWatcherState();
        state->hDir              = hDir;
        state->watchFile         = watchFile;
        state->rootPath          = dirPath;
        state->filterFileName    = fileName;
        state->notifyFilter      = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE;
        state->pendingIo         = false;
        ZeroMemory(&state->overlapped, sizeof(state->overlapped));
        ZeroMemory(state->buffer, sizeof(state->buffer));

        auto* watcher    = new RZFileWatcher();
        watcher->platform = state;
        watcher->poll     = WindowsFileWatcherPoll;
        return watcher;
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForDirectory(const RZString& directoryPath)
    {
        return CreateWindowsWatcher(directoryPath, false, RZString());
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForFile(const RZString& filePath)
    {
        // Watch the parent directory and filter by file name
        std::string path = filePath.c_str();
        size_t      sep  = path.find_last_of("/\\");
        std::string dir  = (sep != std::string::npos) ? path.substr(0, sep) : ".";
        std::string file = (sep != std::string::npos) ? path.substr(sep + 1) : path;
        return CreateWindowsWatcher(dir.c_str(), true, file.c_str());
    }

    void RZFileSystem::DestroyFileWatcher(RZFileWatcher* watcher)
    {
        WindowsFileWatcherDestroy(watcher);
    }
}    // namespace Razix

#endif