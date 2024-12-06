#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        // TODO: Rename this to RZConstantBuffer
        /* Uniform Buffer that contains various kinds of data such as animation, lighting etc and can be passed to the shader stages */
        class RAZIX_API RZUniformBuffer : public IRZResource<RZUniformBuffer>
        {
        public:
            /* The API is responsible for deallocating any resources */
            RAZIX_VIRTUAL_DESCTURCTOR(RZUniformBuffer)

            GET_INSTANCE_SIZE;

            /* Binds the buffer to the pipeline (relevant for DX and GL) */
            virtual void Bind(/*RZPipeline* pipeline, RZCommandBuffer& cmdBuffer*/) = 0;
            /**
             * Sets the data contents of the uniform buffer
             * 
             * @param size The size of the uniform buffer
             * @param data The data being stored in the uniform buffer
             */
            virtual void SetData(u32 size, const void* data) = 0;
            virtual void Flush()                             = 0;
            virtual void Invalidate()                        = 0;

        protected:
            RZBufferDesc m_Desc;

        private:
            /**
             * Creates a uniform buffer that can be used to send data to the shaders using the underlying Graphics API
             * 
             * @parma size The size of the uniform buffer
             * @parma data The data being stored in the uniform buffer
             * @returns The pointer handle to underlying Uniform buffer API implementation
             */
            static void Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Graphics
}    // namespace Razix
