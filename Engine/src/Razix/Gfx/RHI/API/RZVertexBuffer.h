#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Gfx {

        /* Forward declaring for reducing coupling */
        class RZDrawCommandBuffer;

        /* Vertex buffer that contains the vertex data that will be used to render geometry */
        class RAZIX_API RZVertexBuffer : public IRZResource<RZVertexBuffer>
        {
        public:
            RZVertexBuffer() = default;
            RAZIX_VIRTUAL_DESCTURCTOR(RZVertexBuffer)

            GET_INSTANCE_SIZE;

            /**
             * Binds the given Vertex buffer to the graphics pipeline before a draw command is issued
             * 
             * @param The Command Buffer that will be used to bind/draw with
             */
            virtual void Bind(RZDrawCommandBufferHandle cmdBuffer) = 0;
            /* Unbinds the vertex buffer */
            virtual void Unbind() = 0;
            /* Sets the data with some offset */
            virtual void SetData(u32 size, const void* data) = 0;
            /* Resizes and sets the buffer with new data */
            virtual void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) = 0;

            virtual void Map(u32 size = 0, u32 offset = 0) = 0;
            virtual void UnMap()                           = 0;
            /**
             * Gets the region on the HOST to which the device memory was mapped to
             * 
             * @returns The buffer to copy contents onto
             */
            virtual void* GetMappedBuffer() = 0;
            virtual void  Flush()           = 0;
            virtual void  Invalidate()      = 0;

        protected:
            RZBufferDesc m_Desc;

        private:
            /**
             * Creates a vertex buffer with the specified usage
             * 
             * @param usage The usage Description of the buffer
             */
            static void Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };

    }    // namespace Gfx
}    // namespace Razix
