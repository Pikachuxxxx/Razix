#include "RZMemoryRoot.h"

#include "RZMemoryFunctions.h"

namespace Razix {

    void* RZMemoryRoot::operator new(size_t size)
    {
        return RZMalloc(size);
    }

    void* RZMemoryRoot::operator new[](size_t size)
    {
        return RZMalloc(size);
    }

    void* RZMemoryRoot::operator new[](size_t size, void* where)
    {
        (void) size;
        return (where);
    }

    void RZMemoryRoot::operator delete(void* pointer)
    {
        RZFree(pointer);
    }

    void RZMemoryRoot::operator delete[](void* pointer)
    {
        RZFree(pointer);
    }
}