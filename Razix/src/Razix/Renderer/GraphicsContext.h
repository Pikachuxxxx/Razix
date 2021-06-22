#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

namespace Razix
{
    class RAZIX_API GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };
}