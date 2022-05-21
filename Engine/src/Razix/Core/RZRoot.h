#pragma once

#include <internal/RazixMemory/include/RZMemory.h>

namespace Razix {

    /**
     * The base class from which all the Razix classes derive from 
     * 
     * This is done to provide common functionality all over the engine
     */
    class RZRoot : public RZMemoryRoot
    {
    public:
        RZRoot() = default;
        virtual ~RZRoot() { }
    };
}