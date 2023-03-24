// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVertexBufferLayout.h"

namespace Razix {
    namespace Graphics {

        RZVertexBufferLayout::RZVertexBufferLayout()
            : m_Stride(0) {}

        void RZVertexBufferLayout::pushImpl(const std::string& name, BufferFormat format, u32 size, bool Normalised)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Elements.push_back({name, format, m_Stride, Normalised});
            m_Stride += size;
        }

        template<>
        void RZVertexBufferLayout::push<int8_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R8_INT, sizeof(int8_t), normalized);
        }

        template<>
        void RZVertexBufferLayout::push<u8>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R8_UINT, sizeof(u8), normalized);
        }

        template<>
        void RZVertexBufferLayout::push<int32_t>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_INT, sizeof(int32_t), normalized);
        }

        template<>
        void RZVertexBufferLayout::push<u32>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_UINT, sizeof(u32), normalized);
        }

        template<>
        void RZVertexBufferLayout::push<f32>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32_FLOAT, sizeof(f32), normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::vec2>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32_FLOAT, sizeof(f32) * 2, normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::vec3>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32_FLOAT, sizeof(f32) * 3, normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::vec4>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32A32_FLOAT, sizeof(f32) * 4, normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::ivec2>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32_INT, sizeof(int) * 2, normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::ivec3>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32_INT, sizeof(int) * 3, normalized);
        }

        template<>
        void RZVertexBufferLayout::push<glm::ivec4>(const std::string& name, bool normalized)
        {
            pushImpl(name, BufferFormat::R32G32B32A32_INT, sizeof(int) * 4, normalized);
        }

        //template <> void RZVertexBufferLayout::push<glm::mat3>(const std::string& name, bool normalized)
        //{
        //    pushImpl(name, BufferFormat::R32G32B32_FLOAT3X3, sizeof(glm::mat3), normalized);
        //}
        //
        //template <> void RZVertexBufferLayout::push<glm::mat4>(const std::string& name, bool normalized)
        //{
        //    pushImpl(name, BufferFormat::R32G32B32A32_FLOAT4X4, sizeof(glm::mat4), normalized);
        //}
    }    // namespace Graphics
}    // namespace Razix