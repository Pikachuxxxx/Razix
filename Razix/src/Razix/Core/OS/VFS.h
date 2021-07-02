#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/OS/FileSystem.h"

namespace Razix
{
	/// <summary>
	/// Cross-platform Virtual File System for the Engine
	/// </summary>
	class VFS
	{
	public:
		/// <summary>
		/// Initializes the VFS
		/// </summary>
		static void StartUp();

		/// <summary>
		/// Shuts down the VFS and releases any resources hold by this
		/// </summary>
		static void ShutDown();

		/// <summary>
		/// Returns the instance to the Static instance of the VFS
		/// </summary>
		/// <returns> Returns a const pointer to the Virtual File System Object </returns>
		inline static const VFS* Get() { return s_Instance; }

		/// <summary>
		/// Mounts the Virtual path along with it's actual physical path so as to resolve 
		/// by just using the relative path into the VFS directory
		/// </summary>
		/// <param name="virtualPath"> The virtual address of the path </param>
		/// <param name="physicalPaht"> The actual physical address of the path on the disk </param>
		void Mount(const std::string& virtualPath, const std::string& physicalPath);

		/// <summary>
		/// Unmount the virtual path from the VFS directory
		/// </summary>
		/// <param name="path"></param>
		void UnMount(const std::string& path);

		/// <summary>
		/// Resolves the complete physical path on the disk of the provided physical path
		/// </summary>
		/// <param name="virtualPath"> The virtual path whose complete address will be resolved </param>
		/// <param name="outPhysicalPath"> The output resolved physical path </param>
		/// <param name="folder"> If false, resolves the path despite the absence of folder </param>
		/// <returns> True if the provided virtual path was properly resolved </returns>
		bool ResolvePhysicalPath(const std::string& virtualPath, std::string& outPhysicalPath, bool folder = false);

		/// <summary>
		/// Converts the absolute path to one of the mounted VFS's virtual paths
		/// </summary>
		/// <param name="absolutePath"> The actual absolute path in the disk </param>
		/// <param name="outVirtualPath"> The output virtual path from the mounted VFS directory </param>
		/// <param name="folder">  If false, resolves the path despite the absence of folder </param>
		/// <returns> True if the provided absolute path was properly resolved into virtual path </returns>
		bool AbsolutePathToVFS(const std::string& absolutePath, std::string& outVirtualPath, bool folder = false);

		/// <summary>
		/// Reads the file into a buffer given the virtual path
		/// </summary>
		/// <param name="path"> The virtual path from which the file is read </param>
		/// <returns> Buffer pointer to contents of the string </returns>
		uint8_t* ReadFile(const std::string& path);

		/// <summary>
		/// Reads a text file from the specified virtual path
		/// </summary>
		/// <param name="path"> The virtual path from which the file is read </param>
		/// <returns> A string containing the contents of the text file </returns>
		std::string ReadTextFile(const std::string& path);

		/// <summary>
		/// Writes the file into a buffer given the virtual path
		/// </summary>
		/// <param name="path"> The virtual path to which the file is being written to </param>
		/// <returns> True, if the file was written successfully </returns>
		bool WriteFile(const std::string& path, uint8_t* buffer);

		/// <summary>
		/// Writes the text file given the virtual path
		/// </summary>
		/// <param name="path"> The virtual path to which the file is being written to </param>
		/// <returns> True, if the file was written successfully </returns>
		bool WriteTextFile(const std::string& path, const std::string& text);

	private:
		///	The singleton static instance of the class
		static VFS* s_Instance;
		/// The map of virtual path and it's corresponding Physical mount paths in a key value association
		std::unordered_map<std::string, std::vector<std::string>> m_MountPoints;
	private:
	};
	
}

