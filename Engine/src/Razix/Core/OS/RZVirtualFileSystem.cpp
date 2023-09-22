// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVirtualFileSystem.h"

#include "Razix/Core/OS/RZFileSystem.h"
// TODO: Try to remove RZEngine.h from here
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZSplashScreen.h"

#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    // TODO: Move this to explicit lazy singleton instantiation as a member in Engine class
    void RZVirtualFileSystem::StartUp()
    {
        // Instance is automatically created once the system is Started Up
        RAZIX_CORE_INFO("[Virtual File System] Starting Up Virtual File Sytem");

        Razix::RZSplashScreen::Get().setLogString("Starting VFS...");

        // Static Initialization load basic paths + root paths in consoles
        //  1.1. Mount engine specific Paths
        auto& rootDir = RZEngine::Get().getEngineInstallationDir();
        RZVirtualFileSystem::Get().mount("RazixRoot", rootDir + std::string("/Engine/"));
        RZVirtualFileSystem::Get().mount("RazixSource", rootDir + std::string("/Engine/src/"));
        RZVirtualFileSystem::Get().mount("RazixContent", rootDir + std::string("/Engine/content/"));
        RZVirtualFileSystem::Get().mount("RazixShaders", rootDir + std::string("/Engine/content/Shaders/Razix"));
        RZVirtualFileSystem::Get().mount("RazixFG", rootDir + std::string("/Engine/content/FrameGraphs"));
    }

    void RZVirtualFileSystem::ShutDown()
    {
        RAZIX_CORE_ERROR("[Virtual File System] Shutting Down Virtual File System");
    }

    void RZVirtualFileSystem::mount(const std::string& virtualPath, const std::string& physicalPath)
    {
        RAZIX_CORE_TRACE("[Virtual File System] Mounting file path at : {0}", physicalPath);
        m_MountPoints[virtualPath].push_back(physicalPath);

        // Create the Directories as well
        RZFileSystem::CreateDir(physicalPath);
    }

    void RZVirtualFileSystem::unMount(const std::string& path)
    {
        m_MountPoints[path].clear();
    }

    bool RZVirtualFileSystem::resolvePhysicalPath(const std::string& virtualPath, std::string& outPhysicalPath, bool folder /*= false*/)
    {
        // Check if it's the absolute path, if not resolve the virtual path
        if (!(virtualPath[0] == '/' && virtualPath[1] == '/')) {
            outPhysicalPath = virtualPath;
            return folder ? RZFileSystem::FolderExists(virtualPath) : RZFileSystem::FileExists(virtualPath);
        }

        // Break the path by '/' and get the list of directories of the path to search
        static std::string       delimiter  = "/";
        std::vector<std::string> dirs       = Utilities::SplitString(virtualPath, delimiter);
        const std::string&       virtualDir = dirs.front();

        // If it is the last directory it's the path itself
        if (m_MountPoints.find(virtualDir) == m_MountPoints.end() || m_MountPoints[virtualDir].empty()) {
            outPhysicalPath = virtualPath;
            return folder ? RZFileSystem::FolderExists(virtualPath) : RZFileSystem::FileExists(virtualPath);
        }

        // Find the new path from the mount points using the virtual directories
        const std::string remainder = virtualPath.substr(virtualDir.size() + 2, virtualPath.size() - virtualDir.size());
        for (const std::string& physicalPath: m_MountPoints[virtualDir]) {
            const std::string newPath = physicalPath + remainder;
            if (folder ? RZFileSystem::FolderExists(newPath) : RZFileSystem::FileExists(newPath)) {
                outPhysicalPath = newPath;
                return true;
            }
        }
        return false;
    }

    bool RZVirtualFileSystem::absolutePathToVFS(const std::string& absolutePath, std::string& outVirtualPath, bool folder /*= false*/)
    {
        // Find the corresponding virtual path from the mount points using the complete file path
        for (auto const& [key, val]: m_MountPoints) {
            for (auto& vfsPath: val) {
                if (absolutePath.find(vfsPath) != std::string::npos) {
                    std::string newPath     = absolutePath;
                    std::string newPartPath = "//" + key;
                    newPath.replace(0, vfsPath.length(), newPartPath);
                    outVirtualPath = newPath;
                    return true;
                }
            }
        }
        outVirtualPath = absolutePath;
        return false;
    }

    u8* RZVirtualFileSystem::readFile(const std::string& path)
    {
        // RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        std::string physicalPath;
        return resolvePhysicalPath(path, physicalPath) ? RZFileSystem::ReadFile(physicalPath) : nullptr;
    }

    std::string RZVirtualFileSystem::readTextFile(const std::string& path)
    {
        // RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        std::string physicalPath;
        return resolvePhysicalPath(path, physicalPath) ? RZFileSystem::ReadTextFile(physicalPath) : nullptr;
    }

    bool RZVirtualFileSystem::writeFile(const std::string& path, u8* buffer)
    {
        // RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        std::string physicalPath;
        return resolvePhysicalPath(path, physicalPath) ? RZFileSystem::WriteFile(physicalPath, buffer) : false;
    }

    bool RZVirtualFileSystem::writeTextFile(const std::string& path, const std::string& text)
    {
        // RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        std::string physicalPath;
        return resolvePhysicalPath(path, physicalPath) ? RZFileSystem::WriteTextFile(physicalPath, text) : false;
    }
}    // namespace Razix
