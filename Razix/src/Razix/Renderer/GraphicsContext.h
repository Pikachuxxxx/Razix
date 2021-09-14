#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

namespace Razix
{
    /* The Render API to use for rendering the application */
    enum class RenderAPI 
    {
        OPENGL = 0,
        VULKAN,
        DIRECTX11,
        DIRECTX12,  // Not Supported yet!
        GXM,        // Not Supported yet!
        GCM         // Not Supported yet!
    };

    /**
     * The Graphics Context that manages the context of the underlying graphics API
     */
    class RAZIX_API GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };
}