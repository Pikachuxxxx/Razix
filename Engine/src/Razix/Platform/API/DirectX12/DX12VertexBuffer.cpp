// clang-format off
#include "rzxpch.h"
// clang-format on

#include "DX12VertexBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Graphics {

        void DX12VertexBuffer::AddBufferLayout(RZVertexBufferLayout& layout)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Bind(RZDrawCommandBufferHandle cmdBuffer)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Flush()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void* DX12VertexBuffer::GetMappedBuffer()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Invalidate()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Map(u32 size /*= 0*/, u32 offset /*= 0*/)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::SetData(u32 size, const void* data)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::UnMap()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

void DX12VertexBuffer::Unbind()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix

#endif