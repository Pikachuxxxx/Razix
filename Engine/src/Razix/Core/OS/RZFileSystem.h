#pragma once

namespace Razix {
    /**
     * Provides a OS independent interface to Interact with the files stored on the host
     */
    class RAZIX_API RZFileSystem : public RZRoot
    {
    public:
        /**
         * Creates a Directory and Folders for a given path
         * 
         * @param path The folder path to create
         * @returns True, if the the directory was successfully created
         */
        static bool CreateDir(const std::string& path);

        /**
         * Checks if the file exists at the given path
         * 
         * @param path The path to check for
         * @returns True, if the file exists at the path
         */
        static bool FileExists(const std::string& path);

        /**
         * Checks if the folder exists at the given path
         * 
         * @param path The path of the folder
         * @returns True, if the folder exists 
         */
        static bool FolderExists(const std::string& path);

        /**
         * Gets the size of the file at the given path
         * 
         * @param path The path to check for
         * @returns The size of the file in bytes
         */
        static int64_t GetFileSize(const std::string& path);

        /**
         * Reads the contents of file onto a string buffer
         * 
         * @param path The path of the file to read
         * @returns Pointer to the contents of the file (8-bit char pointer)
         */
        static u8* ReadFile(const std::string& path);

        /** 
         * Reads the file onto a void buffer with the specified size
         * 
         * @param path      The path of the file to read
         * @param buffer    The buffer to which the file contents are read to
         * @param size      The size of the file
         * @returns True, if the read was successful 
         */
        static bool ReadFile(const std::string& path, void* buffer, int64_t size = -1);

        /**
         * Reads the text file and returns the text string
         * 
         * @param path The path of the file to read
         * @returns The string buffer containing the contents of the file
         */
        static std::string ReadTextFile(const std::string& path);

        /**
         * Writes the file with the specified buffer contents
         * 
         * @param path The path of the file to write to
         * @param buffer The content that will be written to the fire
         * @returns True, if the write operation was successful
         */
        static bool WriteFile(const std::string& path, u8* buffer, i64 size);

        /**
         * Writes the string to a text file
         * 
         * @param path The path of the file to write to
         * @param text The content that will be written in the file
         * @returns  True, if the write was successful
         */
        static bool WriteTextFile(const std::string& path, const std::string& text);

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
    };

}    // namespace Razix
