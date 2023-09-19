#pragma once

#include "Razix/Core/RZRoot.h"

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDebugConfig.h"

#include "Razix/Graphics/RHI/API/Data/RZBufferData.h"
#include "Razix/Graphics/RHI/API/RZVertexBufferLayout.h"

namespace Razix {
    namespace Graphics {

        /* Forward declaring for reducing coupling */
        class RZCommandBuffer;

        /* Vertex buffer that contains the vertex data that will be used to render geometry */
        class RAZIX_API RZVertexBuffer : public RZRoot
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
            static RZVertexBuffer* Create(u32 size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG);

            /**
             * Binds the given Vertex buffer to the graphics pipeline before a draw command is issued
             * 
             * @param The Command Buffer that will be used to bind/draw with
             */
            virtual void Bind(RZCommandBuffer* cmdBuffer) = 0;
            /* Unbinds the vertex buffer */
            virtual void Unbind() = 0;
            /* Sets the data with some offset */
            virtual void SetData(u32 size, const void* data) = 0;
            /* Resizes and sets the buffer with new data */
            virtual void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) = 0;
            /* Sets the vertex buffer layout */
            virtual void AddBufferLayout(RZVertexBufferLayout& layout) = 0;
            /* Destroys the buffer and it's resources allocated by the underlying API */
            virtual void Destroy() = 0;

            virtual void Map(u32 size = 0, u32 offset = 0) = 0;
            virtual void UnMap()                           = 0;
            /**
             * Gets the region on the HOST to which the device memory was mapped to
             * 
             * @returns The buffer to copy contents onto
             */
            virtual void* GetMappedBuffer() = 0;
            virtual void  Flush()           = 0;

            RAZIX_INLINE bool isBufferMaped() const { return m_Mapped; }

        protected:
            BufferUsage m_Usage  = BufferUsage::Static;
            u32         m_Size   = 0;
            bool        m_Mapped = false;
        };

    }    // namespace Graphics
}    // namespace Razix
