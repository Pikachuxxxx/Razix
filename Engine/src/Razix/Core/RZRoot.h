#pragma once

#include "Razix/Core/RZCore.h"

#include "internal/RazixMemory/include/RZMemory.h"

namespace Razix {

    /**
     * The base class from which all the Razix classes derive from 
     * 
     * This is done to provide common functionality all over the engine
     * Keep this simple and use it only for memory and serialization to 
     * minimize inheritance complexity
     */
    class RAZIX_API RZRoot : public RZMemoryRoot
    {
    public:
        RZRoot() = default;
        virtual ~RZRoot() { }
    };
}