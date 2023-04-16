// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Context.h"

namespace Razix {
    namespace Graphics {

        DX12Context::DX12Context(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[D3D12] Window Handle is NULL!");
        }

        void DX12Context::Init()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12Context::Destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12Context::ClearWithColor(f32 r, f32 g, f32 b)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix