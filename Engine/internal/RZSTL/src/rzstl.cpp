#include "RZSTL/rzstl.h"

// This file is used to implement and EASTL based implementations

#if USE_EASTL == 1
    #ifndef DEFINE_OVERLOADS
        #define DEFINE_OVERLOADS

// Override some operators for EASTL
int Vsnprintf8(char* p, size_t n, const char* pFormat, va_list arguments)
{
    return vsnprintf(p, n, pFormat, arguments);
}

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return new uint8_t[size];
};

void* __cdecl operator new[](size_t size, size_t alignment, size_t alignmentOffset,
    const char* pName, int flags, unsigned int debugFlags, const char* file, int line)
{
    return new uint8_t[size];
};

    //void* RAZIX_CALLING_CONVENTION operator new(size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
    //{
    //    return Razix::Memory::RZMalloc(size, file, line, name);
    //}
    //
    //void* RAZIX_CALLING_CONVENTION operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned int debugFlags, const char* file, int line)
    //{
    //    return Razix::Memory::RZMalloc(size, alignment, file, line, pName);
    //}
    //
    //void* RAZIX_CALLING_CONVENTION operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
    //{
    //    return Razix::Memory::RZMalloc(size, file, line, name);
    //};
    //
    //void* RAZIX_CALLING_CONVENTION operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned int debugFlags, const char* file, int line)
    //{
    //    return Razix::Memory::RZMalloc(size, alignment, file, line, pName);
    //};
    //
    //void RAZIX_CALLING_CONVENTION operator delete(void* addr)
    //{
    //    Razix::Memory::RZFree(addr);
    //}
    //
    //void RAZIX_CALLING_CONVENTION operator delete[](void* addr)
    //{
    //    Razix::Memory::RZFree(addr);
    //}

    #endif
#endif