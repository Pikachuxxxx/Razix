#pragma once

namespace Razix {
    namespace Graphics {

        /**
         * Storage buffer that can be read or written by shaders
         */
        class RZStorageBuffer
        {
        public:
            RZStorageBuffer() {}
            ~RZStorageBuffer() {}

            RZStorageBuffer* Create(uint32_t size, const std::string& name);

            virtual void Bind() {}

            virtual void* GetData()                                = 0;
            virtual void  SetData(uint32_t size, const void* data) = 0;
            /* Destroys the uniform buffer and frees the buffer and other resources it's holds such as memory etc. */
            virtual void Destroy() = 0;
        };
    }    // namespace Graphics
}    // namespace Razix
