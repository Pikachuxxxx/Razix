// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/OS/RZFileSystem.h"

#ifdef RAZIX_PLATFORM_UNIX

    #include "Razix/Core/Memory/RZMemoryFunctions.h"

    #include <cstring>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>

namespace Razix {

    bool RZFileSystem::CreateDir(const RZString& path)
    {
        // Using mkdir to create directories
        if (mkdir(path.c_str(), 0755) == 0) {
            return true;
        }
        return false;
    }

    bool RZFileSystem::FileExists(const RZString& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && S_ISREG(buffer.st_mode);
    }

    bool RZFileSystem::FolderExists(const RZString& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode);
    }

    int64_t RZFileSystem::GetFileSize(const RZString& path)
    {
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0) {
            return buffer.st_size;
        }
        return -1;
    }

    u8* RZFileSystem::ReadFile(const RZString& path)
    {
        // Get file size first
        int64_t fileSize = GetFileSize(path);
        if (fileSize <= 0) {
            return nullptr;
        }

        // Open file for reading
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            return nullptr;
        }

        // Allocate buffer using RZMalloc
        u8* buffer = (u8*) rz_malloc_aligned(fileSize);
        if (!buffer) {
            close(fd);
            return nullptr;
        }

        // Read file contents
        ssize_t bytesRead = read(fd, buffer, fileSize);
        close(fd);

        if (bytesRead == fileSize) {
            return buffer;
        }

        rz_free(buffer);
        return nullptr;
    }

    bool RZFileSystem::ReadFile(const RZString& path, void* buffer, int64_t size)
    {
        if (!buffer) {
            return false;
        }

        // If size is -1, get the actual file size
        if (size == -1) {
            size = GetFileSize(path);
            if (size <= 0) {
                return false;
            }
        }

        // Open file for reading
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            return false;
        }

        // Read file contents
        ssize_t bytesRead = read(fd, buffer, size);
        close(fd);

        return bytesRead == size;
    }

    RZString RZFileSystem::ReadTextFile(const RZString& path)
    {
        int64_t fileSize = GetFileSize(path);
        if (fileSize <= 0) {
            RAZIX_CORE_ERROR("[FileSystem] cannot open file check path: {0}", path);
            return RZString();
        }

        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            RAZIX_CORE_ERROR("[FileSystem] cannot open file check path: {0}", path);
            return RZString();
        }

        // Allocate temporary buffer using RZMalloc
        char* tempBuffer = (char*) rz_malloc_aligned(fileSize + 1);
        if (!tempBuffer) {
            close(fd);
            return RZString();
        }

        ssize_t bytesRead = read(fd, tempBuffer, fileSize);
        close(fd);

        RZString result;
        if (bytesRead == fileSize) {
            tempBuffer[fileSize] = '\0';
            result               = RZString(tempBuffer);
        }

        rz_free(tempBuffer);
        return result;
    }

    bool RZFileSystem::WriteFile(const RZString& path, u8* buffer, i64 size)
    {
        if (!buffer || size <= 0) {
            return false;
        }

        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            return false;
        }

        ssize_t bytesWritten = write(fd, buffer, size);
        close(fd);

        return bytesWritten == size;
    }

    bool RZFileSystem::WriteTextFile(const RZString& path, const RZString& text)
    {
        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            return false;
        }

        const char* data = text.c_str();
        int64_t     size = text.length();

        ssize_t bytesWritten = write(fd, data, size);
        close(fd);

        return bytesWritten == size;
    }

    //--------------------------------------------------------------------------
    // File Watcher
    //--------------------------------------------------------------------------

    #ifdef RAZIX_PLATFORM_LINUX

        #include <limits.h>
        #include <string>
        #include <sys/inotify.h>

    struct UnixFileWatcherState
    {
        int         inotifyFd;
        int         watchDescriptor;
        bool        watchFile;
        std::string rootPath;
        std::string filterFileName;
    };

    static void UnixFileWatcherPoll(RZFileWatcher* watcher, RZFileChange* outChanges, int* inOutCount, int maxChanges)
    {
        if (!watcher || !watcher->platform || !outChanges || !inOutCount)
            return;

        auto* state = static_cast<UnixFileWatcherState*>(watcher->platform);
        *inOutCount  = 0;

        // Buffer for inotify events
        alignas(struct inotify_event) char buf[sizeof(struct inotify_event) + NAME_MAX + 1];

        while (*inOutCount < maxChanges) {
            ssize_t len = read(state->inotifyFd, buf, sizeof(buf));
            if (len <= 0)
                break;

            const char* ptr = buf;
            while (ptr < buf + len && *inOutCount < maxChanges) {
                const auto* ev = reinterpret_cast<const struct inotify_event*>(ptr);

                if (ev->len > 0) {
                    const char* name = ev->name;

                    // In single-file mode, only process events that match our target file
                    bool shouldProcess = !state->watchFile || state->filterFileName.empty() || (state->filterFileName == name);
                    if (shouldProcess) {
                        RZFileChange& change = outChanges[*inOutCount];
                        std::string   full   = state->rootPath + "/" + name;
                        change.path          = full.c_str();

                        if (ev->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
                            change.type = RZFileChangeType::Modified;
                            ++(*inOutCount);
                        } else if (ev->mask & (IN_CREATE | IN_MOVED_TO)) {
                            change.type = RZFileChangeType::Added;
                            ++(*inOutCount);
                        } else if (ev->mask & (IN_DELETE | IN_MOVED_FROM)) {
                            change.type = RZFileChangeType::Removed;
                            ++(*inOutCount);
                        }
                    }
                }

                ptr += sizeof(struct inotify_event) + ev->len;
            }
        }
    }

    static void UnixFileWatcherDestroy(RZFileWatcher* watcher)
    {
        if (!watcher || !watcher->platform)
            return;

        auto* state = static_cast<UnixFileWatcherState*>(watcher->platform);
        if (state->watchDescriptor >= 0 && state->inotifyFd >= 0)
            inotify_rm_watch(state->inotifyFd, state->watchDescriptor);
        if (state->inotifyFd >= 0)
            close(state->inotifyFd);

        state->~UnixFileWatcherState();
        rz_free(state);
        watcher->~RZFileWatcher();
        rz_free(watcher);
    }

    static RZFileWatcher* CreateLinuxWatcher(const char* dirPath, bool watchFile, const char* fileName)
    {
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd < 0)
            return nullptr;

        int wd = inotify_add_watch(fd, dirPath, IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
        if (wd < 0) {
            close(fd);
            return nullptr;
        }

        void* stateMem = rz_malloc(sizeof(UnixFileWatcherState), alignof(UnixFileWatcherState));
        if (!stateMem) {
            close(fd);
            return nullptr;
        }

        auto* state             = new (stateMem) UnixFileWatcherState();
        state->inotifyFd        = fd;
        state->watchDescriptor  = wd;
        state->watchFile        = watchFile;
        state->rootPath         = dirPath;
        state->filterFileName   = (fileName ? fileName : "");

        void* watcherMem = rz_malloc(sizeof(RZFileWatcher), alignof(RZFileWatcher));
        if (!watcherMem) {
            state->~UnixFileWatcherState();
            rz_free(state);
            inotify_rm_watch(fd, wd);
            close(fd);
            return nullptr;
        }

        auto* w    = new (watcherMem) RZFileWatcher();
        w->platform = state;
        w->poll     = UnixFileWatcherPoll;
        return w;
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForDirectory(const RZString& directoryPath)
    {
        return CreateLinuxWatcher(directoryPath.c_str(), false, nullptr);
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForFile(const RZString& filePath)
    {
        std::string path = filePath.c_str();
        size_t      sep  = path.find_last_of('/');
        std::string dir  = (sep != std::string::npos) ? path.substr(0, sep) : ".";
        std::string file = (sep != std::string::npos) ? path.substr(sep + 1) : path;
        return CreateLinuxWatcher(dir.c_str(), true, file.c_str());
    }

    void RZFileSystem::DestroyFileWatcher(RZFileWatcher* watcher)
    {
        UnixFileWatcherDestroy(watcher);
    }

    #elif defined(RAZIX_PLATFORM_MACOS)

        #include <CoreServices/CoreServices.h>
        #include <dispatch/dispatch.h>
        #include <string>

    struct MacOSFileWatcherState
    {
        FSEventStreamRef stream;
        dispatch_queue_t queue;
        bool             watchFile;
        std::string      rootPath;
        std::string      filterFileName;

        // Ring buffer for collected changes (filled by the FS callback, drained by poll)
        static constexpr int kMaxPending = 256;
        RZFileChange         pending[kMaxPending];
        int                  pendingHead;    // write index
        int                  pendingTail;    // read index
    };

    static void MacOSFSEventCallback(
        ConstFSEventStreamRef          streamRef,
        void*                          clientCallBackInfo,
        size_t                         numEvents,
        void*                          eventPaths,
        const FSEventStreamEventFlags* eventFlags,
        const FSEventStreamEventId*    eventIds)
    {
        (void) streamRef;
        (void) eventIds;

        auto*  state = static_cast<MacOSFileWatcherState*>(clientCallBackInfo);
        char** paths = (char**) eventPaths;

        for (size_t i = 0; i < numEvents; ++i) {
            const char* fullPath = paths[i];

            // In file-watch mode, skip if not our target file
            if (state->watchFile && !state->filterFileName.empty()) {
                std::string p(fullPath);
                size_t      sep  = p.find_last_of('/');
                std::string name = (sep != std::string::npos) ? p.substr(sep + 1) : p;
                if (name != state->filterFileName)
                    continue;
            }

            int next = (state->pendingHead + 1) % MacOSFileWatcherState::kMaxPending;
            if (next == state->pendingTail)
                continue;    // ring buffer full – drop newest event

            RZFileChange& change = state->pending[state->pendingHead];
            change.path          = fullPath;

            FSEventStreamEventFlags flags = eventFlags[i];
            if (flags & kFSEventStreamEventFlagItemCreated)
                change.type = RZFileChangeType::Added;
            else if (flags & kFSEventStreamEventFlagItemRemoved)
                change.type = RZFileChangeType::Removed;
            else
                change.type = RZFileChangeType::Modified;

            state->pendingHead = next;
        }
    }

    static void MacOSDispatchBarrierCallback(void* /*context*/) {}

    static void MacOSFileWatcherPoll(RZFileWatcher* watcher, RZFileChange* outChanges, int* inOutCount, int maxChanges)
    {
        if (!watcher || !watcher->platform || !outChanges || !inOutCount)
            return;

        auto* state = static_cast<MacOSFileWatcherState*>(watcher->platform);
        *inOutCount  = 0;

        // Ask FSEvents to flush any pending events to the dispatch queue.
        FSEventStreamFlushSync(state->stream);
        // Wait for all work currently on the dispatch queue (including the callback)
        // to complete before we read from the ring buffer.
        dispatch_sync_f(state->queue, nullptr, MacOSDispatchBarrierCallback);

        while (state->pendingTail != state->pendingHead && *inOutCount < maxChanges) {
            outChanges[*inOutCount] = state->pending[state->pendingTail];
            ++(*inOutCount);
            state->pendingTail = (state->pendingTail + 1) % MacOSFileWatcherState::kMaxPending;
        }
    }

    static void MacOSFileWatcherDestroy(RZFileWatcher* watcher)
    {
        if (!watcher || !watcher->platform)
            return;

        auto* state = static_cast<MacOSFileWatcherState*>(watcher->platform);
        if (state->stream) {
            FSEventStreamStop(state->stream);
            FSEventStreamInvalidate(state->stream);
            FSEventStreamRelease(state->stream);
        }
        if (state->queue) {
            dispatch_release(state->queue);
        }

        state->~MacOSFileWatcherState();
        rz_free(state);
        watcher->~RZFileWatcher();
        rz_free(watcher);
    }

    static RZFileWatcher* CreateMacOSWatcher(const char* dirPath, bool watchFile, const char* fileName)
    {
        CFStringRef     pathStr   = CFStringCreateWithCString(kCFAllocatorDefault, dirPath, kCFStringEncodingUTF8);
        CFArrayRef      paths     = CFArrayCreate(kCFAllocatorDefault, (const void**) &pathStr, 1, &kCFTypeArrayCallBacks);

        void* stateMem = rz_malloc(sizeof(MacOSFileWatcherState), alignof(MacOSFileWatcherState));
        if (!stateMem) {
            CFRelease(paths);
            CFRelease(pathStr);
            return nullptr;
        }

        auto* state              = new (stateMem) MacOSFileWatcherState();
        state->watchFile         = watchFile;
        state->rootPath          = dirPath;
        state->filterFileName    = (fileName ? fileName : "");
        state->pendingHead       = 0;
        state->pendingTail       = 0;
        state->queue             = nullptr;

        FSEventStreamContext ctx = {0, state, nullptr, nullptr, nullptr};
        state->stream            = FSEventStreamCreate(
            kCFAllocatorDefault,
            &MacOSFSEventCallback,
            &ctx,
            paths,
            kFSEventStreamEventIdSinceNow,
            0.05,
            kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer);

        CFRelease(paths);
        CFRelease(pathStr);

        if (!state->stream) {
            state->~MacOSFileWatcherState();
            rz_free(state);
            return nullptr;
        }

        // Store the dispatch queue in the state so we can release it on destroy
        dispatch_queue_t queue = dispatch_queue_create("com.razix.filewatcher", DISPATCH_QUEUE_SERIAL);
        state->queue           = queue;
        FSEventStreamSetDispatchQueue(state->stream, queue);
        FSEventStreamStart(state->stream);

        void* watcherMem = rz_malloc(sizeof(RZFileWatcher), alignof(RZFileWatcher));
        if (!watcherMem) {
            FSEventStreamStop(state->stream);
            FSEventStreamInvalidate(state->stream);
            FSEventStreamRelease(state->stream);
            if (state->queue) {
                dispatch_release(state->queue);
            }
            state->~MacOSFileWatcherState();
            rz_free(state);
            return nullptr;
        }

        auto* w    = new (watcherMem) RZFileWatcher();
        w->platform = state;
        w->poll     = MacOSFileWatcherPoll;
        return w;
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForDirectory(const RZString& directoryPath)
    {
        return CreateMacOSWatcher(directoryPath.c_str(), false, nullptr);
    }

    RZFileWatcher* RZFileSystem::CreateFileWatcherForFile(const RZString& filePath)
    {
        std::string path = filePath.c_str();
        size_t      sep  = path.find_last_of('/');
        std::string dir  = (sep != std::string::npos) ? path.substr(0, sep) : ".";
        std::string file = (sep != std::string::npos) ? path.substr(sep + 1) : path;
        return CreateMacOSWatcher(dir.c_str(), true, file.c_str());
    }

    void RZFileSystem::DestroyFileWatcher(RZFileWatcher* watcher)
    {
        MacOSFileWatcherDestroy(watcher);
    }

    #else
        #error "RAZIX_PLATFORM_UNIX is defined but neither RAZIX_PLATFORM_LINUX nor RAZIX_PLATFORM_MACOS is defined"
    #endif    // RAZIX_PLATFORM_LINUX / RAZIX_PLATFORM_MACOS

}    // namespace Razix

#endif
