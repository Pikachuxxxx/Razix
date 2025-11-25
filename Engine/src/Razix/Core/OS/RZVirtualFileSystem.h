#pragma once

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"

#include "Razix/Core/Utils/TRZSingleton.h"

namespace Razix {
    /* Cross - Platform Virtual File System for the Engine */
    // TODO: Log unsuccessful path resolution
    // TODO: Add read/write only access
    class RAZIX_API RZVirtualFileSystem : public RZSingleton<RZVirtualFileSystem>
    {
    public:
        /* Initializes the VFS */
        void StartUp();
        /* Shuts down the VFSand releases any resources hold by this */
        void ShutDown();

        /**
         * Mounts the Virtual path along with it's actual physical path so as to resolve
         * by just using the relative path into the VFS directory
         *
         * @param virtualPath The virtual address of the path
         * @param physicalPaht The actual physical address of the path on the disk
		 */
        void mount(const RZString& virtualPath, const RZString& physicalPath);
        /**
		 * Unmount the virtual path from the VFS directory
		 * @param path The path to unmount from the VFS
		 */
        void unMount(const RZString& path);

        /** 
		 * Resolves the complete physical path on the disk of the provided physical path
		 * 
		 * @param virtualPath The virtual path whose complete address will be resolved  
		 * @param outPhysicalPath The output resolved physical path  
		 * @param folder If false, resolves the path despite the absence of folder  
		 * @returns True, if the provided virtual path was properly resolved
	     */
        bool resolvePhysicalPath(const RZString& virtualPath, RZString& outPhysicalPath, bool folder = false);
        /**
		 * Converts the absolute path to one of the mounted VFS's virtual paths
		 * 
		 * @param absolutePath The actual absolute path in the disk  
		 * @param outVirtualPath The output virtual path from the mounted VFS directory  
		 * @param folder If false, resolves the path despite the absence of folder  
		 * @returns True if the provided absolute path was properly resolved into virtual path  
		 */
        bool absolutePathToVFS(const RZString& absolutePath, RZString& outVirtualPath, bool folder = false);

        /**
		 * Reads the file into a buffer given the virtual path
		 * 
		 * @param path The virtual path from which the file is read  
		 * @returns Buffer pointer to contents of the string  
		 */
        u8* readFile(const RZString& path);
        /**
		 * Reads a text file from the specified virtual path
		 * 
		 * @param path The virtual path from which the file is read  
		 * @returns A string containing the contents of the text file 
		 */
        RZString readTextFile(const RZString& path);
        /**
		 * Writes the file into a buffer given the virtual path
		 * 
		 * @param path The virtual path to which the file is being written to  
		 * @returns True, if the file was written successfully  
		 */
        bool writeFile(const RZString& path, u8* buffer, i64 size);
        /**
		 * Writes the text file given the virtual path
		 * 
		 * @param path The virtual path to which the file is being written to  
		 * @returns True, if the file was written successfully  
		 */
        bool writeTextFile(const RZString& path, const RZString& text);

    private:
        RZHashMap<RZString, RZDynamicArray<RZString>> m_MountPoints; /* The map of virtual path and it's corresponding Physical mount paths in a key value association */
    };

}    // namespace Razix
