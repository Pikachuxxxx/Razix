#pragma once

namespace Razix {
    namespace Graphics {

        /* Uniform Buffer that contains various kinds of data such as animation, lighting etc and can be passed to the shader stages */
        class RZUniformBuffer
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
            static RZUniformBuffer* Create(uint32_t size, const void* data);
            
            /* Initializes the uniform buffer */
            virtual void Init() = 0;
            /**
             * Sets the data contents of the uniform buffer
             * 
             * @param size The size of the uniform buffer
             * @param data The data being stored in the uniform buffer
             */ 
            virtual void SetData(uint32_t size, const void* data) = 0;
            /* Destroys the uniform buffer and frees the buffer and other resources it's holds such as memory etc. */
            virtual void Destroy() = 0;
        };
    }    
}

