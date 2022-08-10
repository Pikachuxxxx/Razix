#pragma once

namespace Razix {

    /**
     * Base memory class for all the classes to aid with custom allocation, provides custom operators and allocation functions
     * 
     * we will use macros and other special options to decide which pool will the object be allocated from
     */
    class RZMemoryRoot
    {
    public:
        RZMemoryRoot() = default;
        ~RZMemoryRoot() { } // Having this virtual will add up a vtable pointer, anyway razix initializes and destroys manually so no need to worry about destructor being virtual here

        /**
         * new operator to allocate memory for the Object 
         * 
         * @param size The size of memory to allocate for the object
         */
        void* operator new(size_t size);
        void* operator new[](size_t size);
        void* operator new[](size_t size, void* where);

        void operator delete(void* pointer);
        void operator delete[](void* pointer);
    };
}