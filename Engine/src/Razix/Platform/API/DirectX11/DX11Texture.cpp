// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX11Texture.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

namespace Razix {
    namespace Graphics {

        void DX11Texture::SetData(const void* pixels)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        int32_t DX11Texture::ReadPixels(u32 x, u32 y)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX11Texture::DestroyResource()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX11Texture::Bind(u32 slot)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX11Texture::Unbind(u32 slot)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void* DX11Texture::GetAPIHandlePtr() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }
    }    // namespace Graphics
}    // namespace Razix
#endif