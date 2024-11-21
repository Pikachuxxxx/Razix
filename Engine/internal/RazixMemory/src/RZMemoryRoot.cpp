#include "RZMemoryRoot.h"

#include "RZMemoryFunctions.h"

namespace Razix {
#if 1

    void* RZMemoryRoot::operator new(size_t size)
    {
        return Memory::RZMalloc(size);
    }

    void* RZMemoryRoot::operator new[](size_t size)
    {
        return Memory::RZMalloc(size);
    }

    void* RZMemoryRoot::operator new[](size_t size, void* where)
    {
        (void) size;
        return (where);
    }

    void RZMemoryRoot::operator delete(void* pointer)
    {
        Memory::RZFree(pointer);
    }

    void RZMemoryRoot::operator delete[](void* pointer)
    {
        Memory::RZFree(pointer);
    }
#endif
}