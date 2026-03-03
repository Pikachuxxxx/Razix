#pragma once

#include "Razix/Core/Containers/string.h"

namespace Razix {

    /**
     * Describes the kind of change detected by an RZFileWatcher
     */
    enum class RZFileChangeType
    {
        Modified,
        Added,
        Removed
    };

    /**
     * Represents a single file change event returned by RZFileWatcher::poll
     */
    struct RZFileChange
    {
        RZString       path;
        RZFileChangeType type;
    };

    /**
     * Opaque file-watcher handle.
     * One watcher per root path (file or directory).
     * The caller owns the watcher and must call RZFileSystem::DestroyFileWatcher when done.
     */
    struct RZFileWatcher;

    using RZFileWatcherPollFn    = void (*)(RZFileWatcher* watcher, RZFileChange* outChanges, int* inOutCount, int maxChanges);
    using RZFileWatcherDestroyFn = void (*)(RZFileWatcher* watcher);

    struct RZFileWatcher
    {
        void*                platform;    // OS-specific state
        RZFileWatcherPollFn  poll;
        RZFileWatcherDestroyFn destroy;
    };

    /**
     * Provides a OS independent interface to Interact with the files stored on the host
     */
    class RAZIX_API RZFileSystem
    {
    public:
        /**
         * Creates a Directory and Folders for a given path
         * 
         * @param path The folder path to create
         * @returns True, if the the directory was successfully created
         */
        static bool CreateDir(const RZString& path);

        /**
         * Checks if the file exists at the given path
         * 
         * @param path The path to check for
         * @returns True, if the file exists at the path
         */
        static bool FileExists(const RZString& path);

        /**
         * Checks if the folder exists at the given path
         * 
         * @param path The path of the folder
         * @returns True, if the folder exists 
         */
        static bool FolderExists(const RZString& path);

        /**
         * Gets the size of the file at the given path
         * 
         * @param path The path to check for
         * @returns The size of the file in bytes
         */
        static int64_t GetFileSize(const RZString& path);

        /**
         * Reads the contents of file onto a string buffer
         * 
         * @param path The path of the file to read
         * @returns Pointer to the contents of the file (8-bit char pointer)
         */
        static u8* ReadFile(const RZString& path);

        /** 
         * Reads the file onto a void buffer with the specified size
         * 
         * @param path      The path of the file to read
         * @param buffer    The buffer to which the file contents are read to
         * @param size      The size of the file
         * @returns True, if the read was successful 
         */
        static bool ReadFile(const RZString& path, void* buffer, int64_t size = -1);

        /**
         * Reads the text file and returns the text string
         * 
         * @param path The path of the file to read
         * @returns The string buffer containing the contents of the file
         */
        static RZString ReadTextFile(const RZString& path);

        /**
         * Writes the file with the specified buffer contents
         * 
         * @param path The path of the file to write to
         * @param buffer The content that will be written to the fire
         * @returns True, if the write operation was successful
         */
        static bool WriteFile(const RZString& path, u8* buffer, i64 size);

        /**
         * Writes the string to a text file
         * 
         * @param path The path of the file to write to
         * @param text The content that will be written in the file
         * @returns  True, if the write was successful
         */
        static bool WriteTextFile(const RZString& path, const RZString& text);

        /* Check if the provided path was a relative path or an absolute path */
        static bool IsRelativePath(cstr path)
        {
            if (!path || path[0] == '/' || path[0] == '\\') {
                return false;
            }

            if (strlen(path) >= 2 && isalpha(path[0]) && path[1] == ':') {
                return false;
            }

            return true;
        }

        /* Tells if the path was an absolute path or not */
        static bool IsAbsolutePath(cstr path)
        {
            if (!path) {
                return false;
            }

            return !IsRelativePath(path);
        }

        /**
         * Creates a file watcher that monitors the given directory for changes.
         * The caller owns the returned watcher and must destroy it with DestroyFileWatcher.
         *
         * @param directoryPath  Absolute or relative path to the directory to watch
         * @returns Pointer to a new RZFileWatcher, or nullptr on failure
         */
        static RZFileWatcher* CreateFileWatcherForDirectory(const RZString& directoryPath);

        /**
         * Creates a file watcher that monitors a single file for changes.
         * The caller owns the returned watcher and must destroy it with DestroyFileWatcher.
         *
         * @param filePath  Absolute or relative path to the file to watch
         * @returns Pointer to a new RZFileWatcher, or nullptr on failure
         */
        static RZFileWatcher* CreateFileWatcherForFile(const RZString& filePath);

        /**
         * Destroys a file watcher previously created by CreateFileWatcherForDirectory
         * or CreateFileWatcherForFile and releases all associated OS resources.
         *
         * @param watcher  The watcher to destroy; must not be used after this call
         */
        static void DestroyFileWatcher(RZFileWatcher* watcher);
    };

}    // namespace Razix
