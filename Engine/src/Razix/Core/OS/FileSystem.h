#pragma once

namespace Razix
{
	/// <summary>
	/// Provides a OS independent interface to Interact with the files stored on the host
	/// </summary>
	class FileSystem
	{
	public:
		/// <summary>
		/// Checks if the file exists at the given path
		/// </summary>
		/// <param name="path"> The path to check for </param>
		/// <returns> True, if the file exists at the path </returns>
		static bool FileExists(const std::string& path);

		/// <summary>
		/// Checks if the folder exists at the given path
		/// </summary>
		/// <param name="path"> The path of the folder </param>
		/// <returns> True, if the folder exists </returns>
		static bool FolderExists(const std::string& path);

		/// <summary>
		/// Gets the size of the file at the given path
		/// </summary>
		/// <param name="path"> The path to check for </param>
		/// <returns> The size of the file in bytes </returns>
		static int64_t GetFileSize(const std::string& path);

		/// <summary>
		/// Reads the contents of file onto a string buffer
		/// </summary>
		/// <param name="path"> The path of the file to read </param>
		/// <returns></returns>
		static uint8_t* ReadFile(const std::string& path);

		/// <summary>
		/// Reads the file onto a void buffer with the specified size
		/// </summary>
		/// <param name="path"> The path of the file to read </param>
		/// <param name="buffer"> The buffer to which the file contents are read to </param>
		/// <param name="size"> The size of the file </param>
		/// <returns> True, if the read was successful </returns>
		static bool ReadFile(const std::string& path, void* buffer, int64_t size = -1);

		/// <summary>
		///  Reads the text file and returns the text string
		/// </summary>
		/// <param name="path"> The path of the file to read </param>
		/// <returns> The string buffer containing the contents of the file </returns>
		static std::string ReadTextFile(const std::string& path);

		/// <summary>
		/// Writes the file with the specified buffer contents
		/// </summary>
		/// <param name="path"> The path of the file to write to </param>
		/// <param name="buffer"> The content that will be written in the file </param>
		/// <returns> True, if the write was successful </returns>
		static bool WriteFile(const std::string& path, uint8_t* buffer);

		/// <summary>
		/// Writes the string to a text file
		/// </summary>
		/// <param name="path"> The path of the file to write to </param>
		/// <param name="text"> The content that will be written in the file </param>
		/// <returns>  True, if the write was successful </returns>
		static bool WriteTextFile(const std::string& path, const std::string& text);

		/// <summary>
		/// Check if the provided path was a relative path or an absolute path
		/// </summary>
		static bool IsRelativePath(const char* path)
		{
			if (!path || path[0] == '/' || path[0] == '\\')
			{
				return false;
			}

			if (strlen(path) >= 2 && isalpha(path[0]) && path[1] == ':')
			{
				return false;
			}

			return true;
		}

		/// <summary>
		/// Tells if the path was an absolute path or not
		/// </summary>
		static bool IsAbsolutePath(const char* path)
		{
			if (!path)
			{
				return false;
			}

			return !IsRelativePath(path);
		}
	};
}

