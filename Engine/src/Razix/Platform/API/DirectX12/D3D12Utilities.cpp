// clang-format off
#include "rzxpch.h"
// clang-format on
#include "D3D12Utilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Graphics {
        namespace D3D12Utilities {

            void TransitionResource(ID3D12GraphicsCommandList2* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
            {
                D3D12_RESOURCE_BARRIER barrier{};
                barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource   = resource;
                barrier.Transition.StateBefore = beforeState;
                barrier.Transition.StateAfter  = afterState;
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

                commandList->ResourceBarrier(1, &barrier);
            }

            void GetCPUDescriptorOffsetHandle(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, INT offsetInDescriptors, UINT descriptorIncrementSize)
            {
                handle.ptr = SIZE_T(INT64(handle.ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
            }

        }    // namespace D3D12Utilities
    }        // namespace Graphics
}    // namespace Razix

#endif