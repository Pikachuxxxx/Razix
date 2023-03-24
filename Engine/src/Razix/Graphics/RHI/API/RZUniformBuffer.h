#pragma once

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        // TODO: Add bind method to RZUniformBuffer
        /* Uniform Buffer that contains various kinds of data such as animation, lighting etc and can be passed to the shader stages */
        class RAZIX_API RZUniformBuffer : public RZRoot
        {
        public:
            /* The API is responsible for deallocating any resources */
            virtual ~RZUniformBuffer() = default;

            /**
             * Creates a uniform buffer that can be used to send data to the shaders using the underlying Graphics API
             * 
             * @parma size The size of the uniform buffer
             * @parma data The data being stored in the uniform buffer
             * @returns The pointer handle to underlying Uniform buffer API implementation
             */
            static RZUniformBuffer* Create(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG);

            /* Initializes the uniform buffer */
            virtual void Init(const void* data RZ_DEBUG_NAME_TAG_E_ARG) = 0;
            /* Binds the buffer to the pipeline (relevant for DX and GL) */
            virtual void Bind() = 0;
            /**
             * Sets the data contents of the uniform buffer
             * 
             * @param size The size of the uniform buffer
             * @param data The data being stored in the uniform buffer
             */
            virtual void SetData(u32 size, const void* data) = 0;
            /* Destroys the uniform buffer and frees the buffer and other resources it's holds such as memory etc. */
            virtual void Destroy() = 0;
        };
    }    // namespace Graphics
}    // namespace Razix
