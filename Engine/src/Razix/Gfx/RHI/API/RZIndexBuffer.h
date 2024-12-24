#pragma once

#include "Razix/Core/RZDebugConfig.h"

#include "Razix/Gfx/Resources/IRZResource.h"

#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

namespace Razix {
    namespace Gfx {

        /* Creates an Index Buffer to use indexed data to draw geometry */
        class RAZIX_API RZIndexBuffer : public IRZResource<RZIndexBuffer>
        {
        public:
            RZIndexBuffer() = default;
            /* Virtual destructor enables the API implementation to delete it's resources */
            RAZIX_VIRTUAL_DESCTURCTOR(RZIndexBuffer)
            RAZIX_NONCOPYABLE_CLASS(RZIndexBuffer)

            GET_INSTANCE_SIZE;

            /* Binds the Index buffer to the pipeline and the command buffer that is recorded and binded with */
            virtual void Bind(RZDrawCommandBufferHandle cmdBuffer = {}) = 0;
            /* Unbinds the index buffer */
            virtual void Unbind() = 0;
            /* Resizes the buffer with new data */
            virtual void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) = 0;

            virtual void  Map(u32 size = 0, u32 offset = 0) = 0;
            virtual void  UnMap()                           = 0;
            virtual void* GetMappedBuffer()                 = 0;
            virtual void  Flush()                           = 0;
            virtual void  Invalidate()                      = 0;

            /* Gets the index count of the index buffer */
            inline u32 getCount() const { return m_IndexCount; }
            /* Sets the index count of the index buffer */
            inline void setCount(u32 count) { m_IndexCount = count; }

        protected:
            RZBufferDesc m_Desc;
            u32          m_IndexCount; /* The index count of the index buffer  */

        private:
            /**
             * Creates an index buffer with the given index data to render the geometry
             */
            static void Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix
