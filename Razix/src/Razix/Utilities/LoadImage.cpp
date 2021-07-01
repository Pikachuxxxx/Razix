#include "rzxpch.h"
#include "LoadImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Razix
{

	uint8_t* Utilities::LoadImage(const std::string& filePath, uint32_t* width, uint32_t* height, uint32_t* bpp)
	{
		int texWidth = 0, texHeight = 0, texChannels = 0;
		stbi_uc* pixels = nullptr;
		int sizeOfChannel = 8;
	
		pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, 0);

		// FIXME: Fix the assert system
		//RAZIX_ASSERT((pixels != nullptr), "Could not load image '{0}'!", filePath);

		// TODO: support different texChannels

		if (width)
			*width = texWidth;
		if (height)
			*height = texHeight;

		const int32_t size = texWidth * texHeight * texChannels;
		uint8_t* result = new uint8_t[size];
		memcpy(result, pixels, size);

		stbi_image_free(pixels);
		return result;
	}

}