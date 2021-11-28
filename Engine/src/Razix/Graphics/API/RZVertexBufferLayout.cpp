#include "rzxpch.h"
#include "RZVertexBufferLayout.h"


namespace Razix {
    namespace Graphics {

        RZVertexBufferLayout::RZVertexBufferLayout() : m_Stride(0) { }

        void RZVertexBufferLayout::pushImpl(const std::string& name, BufferFormat format, uint32_t size, bool Normalised)
        {
            m_Elements.push_back({name, format, size, Normalised});
            m_Stride += size;
        }

        template<> void RZVertexBufferLayout::push<int8_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R8_INT, sizeof(int8_t), normalized);
        }

        template<> void RZVertexBufferLayout::push<uint8_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R8_UINT, sizeof(uint8_t), normalized);
        }

        template <> void RZVertexBufferLayout::push<int32_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_INT, sizeof(int32_t), normalized);
        }

        template <> void RZVertexBufferLayout::push<uint32_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_UINT, sizeof(uint32_t), normalized);
        }

        template <> void RZVertexBufferLayout::push<float>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_FLOAT, sizeof(float), normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::vec2>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32_FLOAT, sizeof(float) * 2, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::vec3>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32_FLOAT, sizeof(float) * 3, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::vec4>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32A32_FLOAT, sizeof(float) * 4, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::ivec2>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32_INT, sizeof(int) * 2, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::ivec3>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32_INT, sizeof(int) * 3, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::ivec4>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32A32_INT, sizeof(int) * 4, normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::mat3>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32_FLOAT3X3, sizeof(glm::mat3), normalized);
        }

        template <> void RZVertexBufferLayout::push<glm::mat4>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32A32_FLOAT4X4, sizeof(glm::mat4), normalized);
        }
    }
}