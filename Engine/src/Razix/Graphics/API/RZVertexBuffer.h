#pragma once

#include "Razix/Core/Core.h"

namespace Razix {
    namespace Graphics {

        /* Forward declaring for reducing coupling */
        class RZCommandBuffer;

        /* Defines how the buffer is used */
        enum class BufferUsage
        {
            STATIC,
            DYNAMIC,
            STREAM
        };

        /* Vertex buffer that contains the vertex data that will be used to render geometry */
        class RAZIX_API RZVertexBuffer
        {
        public:
            RZVertexBuffer() = default;
            virtual ~RZVertexBuffer() {}

            /**
             * Creates a vertex buffer with the specified usage
             * 
             * @param usage The usage Description of the buffer
             * @returns Returns a RZVertexBuffer pointer to the underlying Graphics API implementation
             */
            static RZVertexBuffer* Create(BufferUsage& usage);

            /**
             * Binds the given Vertex buffer to the graphics pipeline before a draw command is issued
             * 
             * @param The Command Buffer that will be used to bind/draw with
             */
            virtual void Bind(const RZCommandBuffer* cmdBuffer) = 0;
            /* Unbinds the vertex buffer */
            virtual void Unbind() = 0;
            /* Sets the data for the vertex buffer */
            virtual void SetData(uint32_t size, const void* data) = 0;
            /* Sets the data with some offset */
            virtual void SetSubData(uint32_t size, const void* data, uint32_t offset) = 0;
        protected:
            BufferUsage m_Usage;            
            uint32_t    m_Size;
            bool        m_Mapped = false;
        };

    } 
}
