// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Texture.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Gfx {

        DX12Texture::DX12Texture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
        }

        DX12Texture::DX12Texture(const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG)
        {
        }

        DX12Texture::DX12Texture(ID3D12Resource* backbuffer)
            : m_ResourceHandle(backbuffer)
        {
        }

        void DX12Texture::Bind(u32 slot)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void* DX12Texture::GetAPIHandlePtr() const
        {
            return m_ResourceHandle;
        }

        int32_t DX12Texture::ReadPixels(u32 x, u32 y)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12Texture::Unbind(u32 slot)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12Texture::DestroyResource()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Gfx
}    // namespace Razix

#endif