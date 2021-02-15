#pragma once

#include "Razix/Core.h"
#include "Razix/Log.h"

namespace Razix
{
    class RAZIX_API GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };
}