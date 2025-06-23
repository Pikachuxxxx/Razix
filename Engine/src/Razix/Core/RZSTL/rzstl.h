#pragma once

#include "Razix/Core/RZSTL/bits.h"
#include "Razix/Core/RZSTL/config.h"
#include "Razix/Core/RZSTL/constexpr_map.h"
#include "Razix/Core/RZSTL/map.h"
#include "Razix/Core/RZSTL/ref_counter.h"
#include "Razix/Core/RZSTL/ring_buffer.h"
#include "Razix/Core/RZSTL/smart_pointers.h"
#include "Razix/Core/RZSTL/string.h"
#include "Razix/Core/RZSTL/unordered_map.h"
#include "Razix/Core/RZSTL/vector.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {
        //template<typename T>
        //using initializer_list = std::initializer_list<T>;
    }
}    // namespace Razix

#if USE_EASTL == 1

//int Vsnprintf8(char* p, size_t n, const char* pFormat, va_list arguments);

//void* RAZIX_CALLING_CONVENTION operator new(size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line);
//void* RAZIX_CALLING_CONVENTION operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned int debugFlags, const char* file, int line);
//void* RAZIX_CALLING_CONVENTION operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line);
//void* RAZIX_CALLING_CONVENTION operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned int debugFlags, const char* file, int line);
//
//void RAZIX_CALLING_CONVENTION operator delete(void* addr);
//void RAZIX_CALLING_CONVENTION operator delete[](void* addr);

#endif