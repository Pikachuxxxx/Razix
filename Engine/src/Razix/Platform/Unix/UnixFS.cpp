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

    bool RZFileSystem::WriteFile(const RZString& path, const u8* buffer, i64 size)
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

    RZFileHandle RZFileSystem::OpenFile(const RZString& path, RZFileMode mode)
    {
        int flags = 0;
        switch (mode) {
            case RZFileMode::Read:
                flags = O_RDONLY;
                break;
            case RZFileMode::Write:
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                break;
            case RZFileMode::ReadWrite:
                flags = O_RDWR | O_CREAT;
                break;
            default:
                return RAZIX_INVALID_FILE_HANDLE;
        }

        int fd = open(path.c_str(), flags, 0644);
        if (fd == -1) {
            return RAZIX_INVALID_FILE_HANDLE;
        }
        return static_cast<u64>(fd); 
    }

    void RZFileSystem::CloseFile(const RZFileHandle& fileHandle)
    {
        if (fileHandle != RAZIX_INVALID_FILE_HANDLE) {
            close(fileHandle);
        }
    }

    u32 RZFileSystem::WriteToFile(const RZFileHandle& fileHandle, const void* buffer, size_t size)
    {
        if (fileHandle == RAZIX_INVALID_FILE_HANDLE || !buffer || size == 0) {
            return 0;
        }

        ssize_t bytesWritten = write(fileHandle, buffer, size);
        if (bytesWritten != static_cast<ssize_t>(size)) {
            RAZIX_CORE_ERROR("[FileSystem] Failed to write to file handle: {0}", fileHandle);
            return 0;
        }
        return static_cast<u32>(bytesWritten);
    }

    u32 RZFileSystem::ReadFromFile(const RZFileHandle& fileHandle, void* buffer, size_t size)
    {
        if (fileHandle == RAZIX_INVALID_FILE_HANDLE || !buffer || size == 0) {
            return 0;
        }

        ssize_t bytesRead = read(fileHandle, buffer, size);
        if (bytesRead != static_cast<ssize_t>(size)) {
            RAZIX_CORE_ERROR("[FileSystem] Failed to read from file handle: {0}", fileHandle);
            return 0;
        }
        return static_cast<u32>(bytesRead);
    }

    void RZFileSystem::SeekFile(const RZFileHandle& fileHandle, RZSeekOrigin origin, int64_t offset)
    {
        if (fileHandle == RAZIX_INVALID_FILE_HANDLE) {
            return;
        }

        int whence = 0;
        switch (origin) {
            case RZSeekOrigin::Begin:
                whence = SEEK_SET;
                break;
            case RZSeekOrigin::Current:
                whence = SEEK_CUR;
                break;
            case RZSeekOrigin::End:
                whence = SEEK_END;
                break;
            default:
                return;
        }

        if (lseek(fileHandle, offset, whence) == -1) {
            RAZIX_CORE_ERROR("[FileSystem] Failed to seek in file handle: {0}", fileHandle);
        }
    }
}    // namespace Razix

#endif
