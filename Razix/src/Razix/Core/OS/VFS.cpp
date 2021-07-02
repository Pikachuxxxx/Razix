#include "rzxpch.h"
#include "VFS.h"

#include "Razix/Core/Log.h"
#include "Razix/Core/OS/FileSystem.h"
#include "Razix/Utilities/StringUtilities.h"

namespace Razix
{
    VFS* VFS::s_Instance = nullptr;

    void VFS::StartUp()
    {
        RAZIX_CORE_INFO("Strarting Up Virtual File Sytem");
        s_Instance = new VFS();
    }

    void VFS::ShutDown()
    {
        RAZIX_CORE_WARN("Shutting down VFS and unmounting all paths");
        delete s_Instance;
    }

    void VFS::Mount(const std::string& virtualPath, const std::string& physicalPath)
    {
        RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        m_MountPoints[virtualPath].push_back(physicalPath);
    }

    void VFS::UnMount(const std::string& path)
    {
        RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
        m_MountPoints[path].clear();
    }

    bool VFS::ResolvePhysicalPath(const std::string& virtualPath, std::string& outPhysicalPath, bool folder /*= false*/)
    {
        // Check if it's the absolute path, if not resolve the virtual path
        if (!(virtualPath[0] == '/' && virtualPath[1] == '/'))
        {
            outPhysicalPath = virtualPath;
            return folder ? FileSystem::FolderExists(virtualPath) : FileSystem::FileExists(virtualPath);
        }

        // Break the path by '/' and get the list of directories of the path to search
        static std::string delimiter = "/";
        std::vector<std::string> dirs = Utilities::SplitString(virtualPath, delimiter);
        const std::string& virtualDir = dirs.front();

        // If it is the last directory it's the path itself
        if (m_MountPoints.find(virtualDir) == m_MountPoints.end() || m_MountPoints[virtualDir].empty())
        {
            outPhysicalPath = virtualPath;
            return folder ? FileSystem::FolderExists(virtualPath) : FileSystem::FileExists(virtualPath);
        }

        // Find the new path from the mount points using the virtual directories
        const std::string remainder = virtualPath.substr(virtualDir.size() + 2, virtualPath.size() - virtualDir.size());
        for (const std::string& physicalPath : m_MountPoints[virtualDir])
        {
            const std::string newPath = physicalPath + remainder;
            if (folder ? FileSystem::FolderExists(newPath) : FileSystem::FileExists(newPath))
            {
                outPhysicalPath = newPath;
                return true;
            }
        }
        return false;
    }

    bool VFS::AbsolutePathToVFS(const std::string& absolutePath, std::string& outVirtualPath, bool folder /*= false*/)
    {
        // Find the corresponding virtual path from the mount points using the complete file path
        for (auto const& [key, val] : m_MountPoints)
        {
            for (auto& vfsPath : val)
            {
                if (absolutePath.find(vfsPath) != std::string::npos)
                {
                    std::string newPath = absolutePath;
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

    uint8_t* VFS::ReadFile(const std::string& path)
    {
        RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
		std::string physicalPath;
        return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : nullptr;
    }

    std::string VFS::ReadTextFile(const std::string& path)
    {
        RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
		std::string physicalPath;
        return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : nullptr;
    }

    bool VFS::WriteFile(const std::string& path, uint8_t* buffer)
    {
        RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
		std::string physicalPath;
        return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer) : false;
    }

    bool VFS::WriteTextFile(const std::string& path, const std::string& text)
    {
		RAZIX_ASSERT(s_Instance, "VFS was not Started Up properly");
		std::string physicalPath;
        return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text) : false;
    }

}
