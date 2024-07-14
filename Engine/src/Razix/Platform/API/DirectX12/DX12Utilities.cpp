// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Utilities.h"

#include "Razix/Utilities/RZColorUtilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"

    // d3dx12 Helper stuff
    #include <vendor/d3dx12/d3dx12.h>

    #ifndef RAZIX_DISTRIBUTION
        #include <pix3.h>
    #endif

namespace Razix {
    namespace Graphics {
        namespace DX12Utilities {

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

            ID3D12GraphicsCommandList2* BeginSingleTimeCommandBuffer()
            {
                auto device = Graphics::DX12Context::Get()->getDevice();

                // Create a allocator and allocate a command list
                ID3D12CommandAllocator* commandAllocator = nullptr;
                CHECK_HRESULT(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

                ID3D12GraphicsCommandList2* commandList = nullptr;
                CHECK_HRESULT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));

                // Reset the allocator and command list
                CHECK_HRESULT(commandAllocator->Reset());
                CHECK_HRESULT(commandList->Reset(commandAllocator, nullptr));

                // Associate the command allocator with the command list so that it can be
                // retrieved when the command list is executed.
                CHECK_HRESULT(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator));

                return commandList;
            }

            void EndSingleTimeCommandBuffer(ID3D12GraphicsCommandList2* commandList)
            {
                // Execute and wait until this work is done
                commandList->Close();

                ID3D12CommandAllocator* commandAllocator = nullptr;
                UINT                    dataSize         = sizeof(commandAllocator);
                CHECK_HRESULT(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

                ID3D12CommandList* const ppCommandLists[] = {
                    commandList};

                // Execute the work on GPU queue
                DX12Context::Get()->getSingleTimeGraphicsQueue()->ExecuteCommandLists(1, ppCommandLists);

                auto device = Graphics::DX12Context::Get()->getDevice();

                // Wait for the work do be done on this queue
                ID3D12Fence* fence;
                UINT64       fenceValue = 0;
                device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
                fenceValue++;
                DX12Context::Get()->getSingleTimeGraphicsQueue()->Signal(fence, fenceValue);
                // Wait for the fence
                if (fence->GetCompletedValue() < fenceValue) {
                    HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                    RAZIX_CORE_ASSERT(eventHandle, "[D3D12] Cannot create fence handle.");

                    CHECK_HRESULT(fence->SetEventOnCompletion(fenceValue, eventHandle));

                    WaitForSingleObject(eventHandle, INFINITE);
                    CloseHandle(eventHandle);
                }

                // Cleanup
                D3D_SAFE_RELEASE(fence);
                D3D_SAFE_RELEASE(commandList);
                D3D_SAFE_RELEASE(commandAllocator);
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

            void UpdateBufferResource(ID3D12Resource** pDestinationResource, size_t bufferSize, const void* bufferData)
            {
                auto            device                = Graphics::DX12Context::Get()->getDevice();
                auto            copyCommandQueue      = Graphics::DX12Context::Get()->getCopyQueue();
                ID3D12Resource* pIntermediateResource = nullptr;

                auto commandList = BeginSingleTimeCommandBuffer();

                // Create an committed resource for the upload.
                if (bufferData) {
                    CD3DX12_HEAP_PROPERTIES intResourceHeapProps(D3D12_HEAP_TYPE_UPLOAD);
                    D3D12_RESOURCE_DESC     intResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

                    CHECK_HRESULT(device->CreateCommittedResource(
                        &intResourceHeapProps,
                        D3D12_HEAP_FLAG_NONE,
                        &intResourceDesc,
                        D3D12_RESOURCE_STATE_GENERIC_READ,
                        nullptr,
                        IID_PPV_ARGS(&pIntermediateResource)));

                    // Copy data from CPU to GPU via an intermediate resource.
                    D3D12_SUBRESOURCE_DATA subresourceData = {};
                    subresourceData.pData                  = bufferData;
                    subresourceData.RowPitch               = bufferSize;
                    subresourceData.SlicePitch             = subresourceData.RowPitch;

                    UpdateSubresources(commandList,
                        *pDestinationResource,
                        pIntermediateResource,
                        0,
                        0,
                        1,
                        &subresourceData);
                }

                // Wait until this operation is done
                EndSingleTimeCommandBuffer(commandList);
                pIntermediateResource->Release();
            }

            void GetCPUDescriptorOffsetHandle(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, INT offsetInDescriptors, UINT descriptorIncrementSize)
            {
                handle.ptr = SIZE_T(INT64(handle.ptr) + INT64(offsetInDescriptors) * INT64(descriptorIncrementSize));
            }

        }    // namespace D3D12Utilities
    }        // namespace Graphics
}    // namespace Razix

#endif