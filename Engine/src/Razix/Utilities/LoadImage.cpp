#include "rzxpch.h"
#include "LoadImage.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Razix
{

	uint8_t* Utilities::LoadImageData(const std::string& filePath, uint32_t* width, uint32_t* height, uint32_t* bpp)
	{
		std::string physicalPath;
		if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath))
			return nullptr;

		int texWidth = 0, texHeight = 0, texChannels = 0;
		stbi_uc* pixels = nullptr;
	
		pixels = stbi_load(physicalPath.c_str(), &texWidth, &texHeight, &texChannels, 0);

		RAZIX_CORE_ASSERT((pixels != nullptr), "Could not load image from : {0}", physicalPath);

		// TODO: support different texChannels

		if (width)
			*width = texWidth;
		if (height)
			*height = texHeight;
		if (bpp)
			*bpp = texChannels;

		const int32_t size = texWidth * texHeight * texChannels;
		uint8_t* result = new uint8_t[size];
		memcpy(result, pixels, size);

		stbi_image_free(pixels);
		return result;
	}

}