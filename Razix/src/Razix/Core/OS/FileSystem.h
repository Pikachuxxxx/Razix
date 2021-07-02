#pragma once

namespace Razix
{
	/// <summary>
	/// Provides a OS independent interface to Interact with the files stored on the host
	/// </summary>
	class FileSystem
	{
	public:
		static bool FileExists(const std::string& path);
		static bool FolderExists(const std::string& path);
		static int64_t GetFileSize(const std::string& path);

		static uint8_t* ReadFile(const std::string& path);
		static bool ReadFile(const std::string& path, void* buffer, int64_t size = -1);
		static std::string ReadTextFile(const std::string& path);

		static bool WriteFile(const std::string& path, uint8_t* buffer);
		static bool WriteTextFile(const std::string& path, const std::string& text);

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

