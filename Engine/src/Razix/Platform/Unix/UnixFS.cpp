// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/OS/RZFileSystem.h"

#ifdef RAZIX_PLATFORM_UNIX

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

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
        u8* buffer = (u8*)Memory::RZMalloc(fileSize);
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

        Memory::RZFree(buffer);
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
        char* tempBuffer = (char*)Memory::RZMalloc(fileSize + 1);
        if (!tempBuffer) {
            close(fd);
            return RZString();
        }

        ssize_t bytesRead = read(fd, tempBuffer, fileSize);
        close(fd);

        RZString result;
        if (bytesRead == fileSize) {
            tempBuffer[fileSize] = '\0';
            result = RZString(tempBuffer);
        }

        Memory::RZFree(tempBuffer);
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
        int64_t size = text.length();
        
        ssize_t bytesWritten = write(fd, data, size);
        close(fd);

        return bytesWritten == size;
    }

}    // namespace Razix

#endif
