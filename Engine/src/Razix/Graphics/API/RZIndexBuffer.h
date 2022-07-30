#pragma once

#include "Razix/Core/RZRoot.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

namespace Razix {
    namespace Graphics {

        /* Creates an Index Buffer to use indexed data to draw geometry */
        class RAZIX_API RZIndexBuffer : public RZRoot
        {
        public:
            RZIndexBuffer() = default;
            virtual ~RZIndexBuffer() {}

            /**
             * Creates an index buffer with the given index data to render the geometry
             * 
             * @param data The index to fill the buffer with
             * @param count The number of indices used to draw the geometry
             * @param bufferUsage Whether the index buffer is static or dynamically streamed
             * 
             * @returns The pointer to the underlying API implementation
             */
            static RZIndexBuffer* Create(uint16_t* data, uint32_t count, const std::string& name, BufferUsage bufferUsage = BufferUsage::STATIC);

            /* Binds the Index buffer to the pipeline and the command buffer that is recorded and binded with */
            virtual void Bind(RZCommandBuffer* commandBuffer = nullptr) = 0;
            /* Unbinds the index buffer */
            virtual void Unbind() = 0;
            /* Destroys the buffer and it's resources allocated by the underlying API */
            virtual void Destroy() = 0;
            /* Resizes the buffer with new data */
            virtual void Resize(uint32_t size, const void* data) = 0;

            virtual void  Map(uint32_t size = 0, uint32_t offset = 0) = 0;
            virtual void  UnMap()                                     = 0;
            virtual void* GetMappedBuffer()                           = 0;
            virtual void  Flush()                                     = 0;

            /* Gets the index count of the index buffer */
            inline uint32_t getCount() const { return m_IndexCount; }
            /* Sets the index count of the index buffer */
            inline void setCount(uint32_t count) { m_IndexCount = count; }

        protected:
            uint32_t    m_IndexCount; /* The index count of the index buffer  */
            BufferUsage m_Usage;      /* The usage information of the buffer  */
            uint32_t    m_Size;       /* The size of the Index buffer         */
        };
    }    // namespace Graphics
}    // namespace Razix
