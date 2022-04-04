#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Graphics/API/RZVertexBufferLayout.h"

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
            static RZVertexBuffer* Create(uint32_t size, const void* data, BufferUsage usage, const std::string& name);

            /**
             * Binds the given Vertex buffer to the graphics pipeline before a draw command is issued
             * 
             * @param The Command Buffer that will be used to bind/draw with
             */
            virtual void Bind(RZCommandBuffer* cmdBuffer) = 0;
            /* Unbinds the vertex buffer */
            virtual void Unbind() = 0;
            /* Sets the data with some offset */
            virtual void SetData(uint32_t size, const void* data) = 0;
            /* Sets the vertex buffer layout */
            virtual void AddBufferLayout(RZVertexBufferLayout& layout) = 0;
            /* Destroys the buffer and it's resources allocated by the underlying API */
            virtual void Destroy() = 0;

        protected:
            BufferUsage m_Usage;            
            uint32_t    m_Size;
            bool        m_Mapped = false;
        };

    } 
}

