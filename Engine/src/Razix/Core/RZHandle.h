#pragma once

#include "Razix/Core/RZDataTypes.h"

namespace Razix {

    // https://twitter.com/SebAaltonen/status/1534416275828514817
    // https://twitter.com/SebAaltonen/status/1535175559067713536
    /**
         * Handle is a weak pointer like reference to real objects inside a Pool, this forms the basis for various handles
         * 
         * Handles are like weak pointers. The container has an array of generation counters, and the data getter checks whether the generation counters match. 
         * If not, then the slot was deleted (and possibly reused). The getter returns null instead of the data pointer in this case
         */
    template<typename T>
    class RZHandle
    {
    public:
        RZHandle()
            : m_Index(0), m_Generation(0) {}

        bool isValid() const { return m_Generation != 0; }

    private:
        RZHandle(u32 index, u32 generation)
            : m_Index(index), m_Generation(generation) {}

    private:
        u32 m_Index;      /* The array Index of the Handle in the pool it belongs to    */
        u32 m_Generation; /* Generated index for deletion verification                  */

        template<typename U>
        friend class RZPool;
    };
}    // namespace Razix