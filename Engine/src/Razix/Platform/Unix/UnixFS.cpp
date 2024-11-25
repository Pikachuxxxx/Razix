// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/OS/RZFileSystem.h"


#ifdef RAZIX_PLATFORM_UNIX

namespace Razix {

    bool RZFileSystem::CreateDir(const std::string& path)
    {
        // Using mkdir to create directories
        if (mkdir(path.c_str(), 0755) == 0) {
            return true;
        }
        return false;
    }

    bool RZFileSystem::FileExists(const std::string& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && S_ISREG(buffer.st_mode);
    }

    bool RZFileSystem::FolderExists(const std::string& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode);
    }

    int64_t RZFileSystem::GetFileSize(const std::string& path)
    {
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0) {
            return buffer.st_size;
        }
        return -1;
    }

    u8* RZFileSystem::ReadFile(const std::string& path)
    {
        // Open file for reading
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return nullptr;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // Allocate buffer and read the file contents
        u8* buffer = new u8[size];
        if (file.read(reinterpret_cast<char*>(buffer), size)) {
            file.close();
            return buffer;
        }

        file.close();
        delete[] buffer;
        return nullptr;
    }

    bool RZFileSystem::ReadFile(const std::string& path, void* buffer, int64_t size)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        if (size == -1) {
            file.seekg(0, std::ios::end);
            size = file.tellg();
            file.seekg(0, std::ios::beg);
        }

        file.read(reinterpret_cast<char*>(buffer), size);
        file.close();
        return file.gcount() == size;
    }

    std::string RZFileSystem::ReadTextFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open()) {
            RAZIX_CORE_ERROR("[FileSystem] cannot open file check path: {0}", path);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    bool RZFileSystem::WriteFile(const std::string& path, u8* buffer, i64 size)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        file.write(reinterpret_cast<char*>(buffer), size);
        file.close();
        return true;
    }

    bool RZFileSystem::WriteTextFile(const std::string& path, const std::string& text)
    {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }

        file << text;
        file.close();
        return true;
    }

} // namespace Razix
#endif
