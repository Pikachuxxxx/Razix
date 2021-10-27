#pragma once

#include "Razix/Core/Core.h"

namespace Razix
{
	namespace Utilities
	{
		RAZIX_API uint8_t* LoadImageData(const std::string& filePath, uint32_t* width, uint32_t* height, uint32_t* bpp);
	}
}

