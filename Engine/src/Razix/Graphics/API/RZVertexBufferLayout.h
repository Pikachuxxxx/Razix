#pragma once

#include "Razix/Core/RZCore.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Razix {
    namespace Graphics {

        /* Engine internal format of the buffer data */
        enum class BufferFormat
        {
            R8_INT,   /* char*            */
            R8_UINT,  /* unsigned char*   */
            R32_INT,  /* 32-bit integer   */
            R32_UINT, /* 32-bit uint      */
            R32_FLOAT,
            R32G32_INT,
            R32G32_UINT,
            R32G32_FLOAT,
            R32G32B32_INT,
            R32G32B32_UINT,
            R32G32B32_FLOAT,
            R32G32B32A32_INT,
            R32G32B32A32_UINT,
            R32G32B32A32_FLOAT
        };

        /* Describes the format information of each element in the buffer data cluster */
        struct RAZIX_API BufferLayoutElement
        {
            std::string  name;               /* The name of the buffer element ex. position or uv or normal              */
            BufferFormat format;             /* The format of the element data                                           */
            uint32_t     offset     = 0;     /* Offset of the element from the first basically stride                    */
            bool         normalized = false; /* Whether or not the data is normalized or not                             */

            uint32_t getCount()
            {
                switch (format) {
                    case Razix::Graphics::BufferFormat::R8_INT:
                    case Razix::Graphics::BufferFormat::R8_UINT:
                    case Razix::Graphics::BufferFormat::R32_INT:
                    case Razix::Graphics::BufferFormat::R32_UINT:
                    case Razix::Graphics::BufferFormat::R32_FLOAT:
                        return 1;
                        break;
                    case Razix::Graphics::BufferFormat::R32G32_INT:
                    case Razix::Graphics::BufferFormat::R32G32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32_FLOAT:
                        return 2;
                        break;
                    case Razix::Graphics::BufferFormat::R32G32B32_INT:
                    case Razix::Graphics::BufferFormat::R32G32B32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32B32_FLOAT:
                        return 3;
                        break;
                    case Razix::Graphics::BufferFormat::R32G32B32A32_INT:
                    case Razix::Graphics::BufferFormat::R32G32B32A32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32B32A32_FLOAT:
                        return 4;
                        break;
                    default:
                        return 0;
                        break;
                }
            }
        };

        /* Vertex buffer layout describes the layout of the buffer data that is being uploaded to the GPU to be consumed by the Input Assembler */
        class RAZIX_API RZVertexBufferLayout
        {
        public:
            RZVertexBufferLayout();

            /**
             * Creates a layout entry for the buffer element
             * 
             * @note The element data type must be specified, don't use this method as type of the element is not specified 
             */
            template<typename T>
            void push(const std::string& name, bool Normalised = false)
            {
                RAZIX_ASSERT(false, "Unkown buffer element layout type!");
            }

            /* Overriding/Customizing template implementations for the push method explicitly */
            /* 8-bit ASCII buffer element or char */
            template<>
            void push<int8_t>(const std::string& name, bool normalized);
            /* 8-bit unsigned int or unsigned char */
            template<>
            void push<uint8_t>(const std::string& name, bool normalized);
            /* 32-bit integer */
            template<>
            void push<int32_t>(const std::string& name, bool normalized);
            /* 32-bit unsigned integer */
            template<>
            void push<uint32_t>(const std::string& name, bool normalized);
            /* float */
            template<>
            void push<float>(const std::string& name, bool normalized);
            /* vector2 */
            template<>
            void push<glm::vec2>(const std::string& name, bool normalized);
            /* vector3 */
            template<>
            void push<glm::vec3>(const std::string& name, bool normalized);
            /* vector4 */
            template<>
            void push<glm::vec4>(const std::string& name, bool normalized);
            /* ivector2 */
            template<>
            void push<glm::ivec2>(const std::string& name, bool normalized);
            /* ivector3 */
            template<>
            void push<glm::ivec3>(const std::string& name, bool normalized);
            /* ivector4 */
            template<>
            void push<glm::ivec4>(const std::string& name, bool normalized);
            ///* mat3 */
            //template <> void push<glm::mat3>(const std::string& name, bool normalized);
            ///* mat4 */
            //template <> void push<glm::mat4>(const std::string& name, bool normalized);

            /* Gets the elements buffer layout information */
            inline std::vector<BufferLayoutElement>& getElements() { return m_Elements; }
            /* Gets the stride of the buffer layout */
            inline uint32_t getStride() const { return m_Stride; }

        private:
            std::vector<BufferLayoutElement> m_Elements; /* The elements comprising of the buffer data format                */
            uint32_t                         m_Stride;   /* Stride describes the frequency after which the elements repeat   */

        private:
            void pushImpl(const std::string& name, BufferFormat format, uint32_t size, bool Normalised);
        };
    }    // namespace Graphics
}    // namespace Razix
