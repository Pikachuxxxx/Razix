// clang-format off
#include "rzxpch.h"
// clang-format on
#include "D3D12Utilities.h"

#include "Razix/Utilities/RZColorUtilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #ifndef RAZIX_DISTRIBUTION
        #include <pix3.h>
    #endif

namespace Razix {
    namespace Graphics {
        namespace D3D12Utilities {

            static HMODULE WinPixEventRuntimeModule;

            void LoadPIXRuntime()
            {
    #ifndef RAZIX_DISTRIBUTION
                WinPixEventRuntimeModule = LoadLibrary("WinPixEventRuntime.dll");
                if (!WinPixEventRuntimeModule)
                    RAZIX_CORE_WARN("[D3D12] could not load WinPixEventRuntime.");
    #endif    // RAZIX_DISTRIBUTION
            }

            void CmdBeginLabel(ID3D12GraphicsCommandList2* commandList, const std::string& name, glm::vec4 color)
            {
    #ifndef RAZIX_DISTRIBUTION

                if (!WinPixEventRuntimeModule)
                    return;
                BeginEventOnCommandList pixBeginEventOnCommandList = (BeginEventOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXBeginEventOnCommandList");
                if (pixBeginEventOnCommandList)
                    pixBeginEventOnCommandList(commandList, Utilities::ColorToARGB(color), name.c_str());
    #endif
            }

            void CmdInsertLabel(ID3D12GraphicsCommandList2* commandList, const std::string& name, glm::vec4 color)
            {
    #ifndef RAZIX_DISTRIBUTION

                if (!WinPixEventRuntimeModule)
                    return;
                // Only for pix
                SetMarkerOnCommandList pixSetMarkerOnCommandList = (SetMarkerOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXSetMarkerOnCommandList");
                if (pixSetMarkerOnCommandList)
                    pixSetMarkerOnCommandList(commandList, Utilities::ColorToARGB(color), name.c_str());
    #endif
            }

            void CmdEndLabel(ID3D12GraphicsCommandList2* commandList)
            {
    #ifndef RAZIX_DISTRIBUTION

                if (!WinPixEventRuntimeModule)
                    return;
                EndEventOnCommandList pixEndEventOnCommandList = (EndEventOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXEndEventOnCommandList");
                if (pixEndEventOnCommandList)
                    pixEndEventOnCommandList(commandList);
    #endif
            }

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