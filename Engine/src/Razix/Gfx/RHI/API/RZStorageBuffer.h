#pragma once

namespace Razix {
    namespace Gfx {

        /**
         * Storage buffer that can be read or written by shaders
         */
        class RZStorageBuffer
        {
        public:
            RZStorageBuffer() {}
            ~RZStorageBuffer() {}

            RZStorageBuffer* Create(u32 size, const std::string& name);

            virtual void Bind() {}

            virtual void* GetData()                           = 0;
            virtual void  SetData(u32 size, const void* data) = 0;
            /* Destroys the uniform buffer and frees the buffer and other resources it's holds such as memory etc. */
            virtual void Destroy() = 0;
        };
    }    // namespace Gfx
}    // namespace Razix
