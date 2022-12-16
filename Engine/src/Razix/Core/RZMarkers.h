#pragma once

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
#endif

#define RAZIX_MARK_BEGIN(cmdBuf, name, labelColor) VK_BEGIN_MARKER(cmdBuf, name, labelColor)

#define RAZIX_MARK_ADD(cmdBuf, name, labelColor) VK_INSERT_MARKER(cmdbuf, name, labelColor)

#define RAZIX_MARK_END(cmdBuf) VK_END_MARKER(cmdBuf)