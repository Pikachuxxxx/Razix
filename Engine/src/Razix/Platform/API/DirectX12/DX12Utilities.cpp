// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Utilities.h"

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include "Razix/Utilities/RZColorUtilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"

    // d3dx12 Helper stuff
    #include <vendor/d3dx12/d3dx12.h>

    #ifndef RAZIX_DISTRIBUTION
        #include <pix3.h>
    #endif

namespace Razix {
    namespace Gfx {
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
                auto device = Gfx::DX12Context::Get()->getDevice();

                // Create a allocator and allocate a command list
                ID3D12CommandAllocator* commandAllocator = nullptr;
                CHECK_HRESULT(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

                ID3D12GraphicsCommandList2* commandList = nullptr;
                CHECK_HRESULT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));

                // Reset the allocator and command list
                //CHECK_HRESULT(commandAllocator->Reset());
                //CHECK_HRESULT(commandList->Reset(commandAllocator, nullptr));

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

                auto device = Gfx::DX12Context::Get()->getDevice();

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
                auto            device                = Gfx::DX12Context::Get()->getDevice();
                auto            copyCommandQueue      = Gfx::DX12Context::Get()->getCopyQueue();
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

            DXGI_FORMAT GetFormatFromComponentType(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount)
            {
                switch (componentType) {
                    case D3D_REGISTER_COMPONENT_FLOAT32:
                        switch (componentCount) {
                            case 1: return DXGI_FORMAT_R32_FLOAT;
                            case 2: return DXGI_FORMAT_R32G32_FLOAT;
                            case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
                            case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                            default: return DXGI_FORMAT_UNKNOWN;
                        }
                    case D3D_REGISTER_COMPONENT_SINT32:
                        switch (componentCount) {
                            case 1: return DXGI_FORMAT_R32_SINT;
                            case 2: return DXGI_FORMAT_R32G32_SINT;
                            case 3: return DXGI_FORMAT_R32G32B32_SINT;
                            case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
                            default: return DXGI_FORMAT_UNKNOWN;
                        }
                    case D3D_REGISTER_COMPONENT_UINT32:
                        switch (componentCount) {
                            case 1: return DXGI_FORMAT_R32_UINT;
                            case 2: return DXGI_FORMAT_R32G32_UINT;
                            case 3: return DXGI_FORMAT_R32G32B32_UINT;
                            case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
                            default: return DXGI_FORMAT_UNKNOWN;
                        }
                    default:
                        return DXGI_FORMAT_UNKNOWN;
                }
            }

            u32 GetFormatSize(DXGI_FORMAT format)
            {
                switch (format) {
                    case DXGI_FORMAT_R32_FLOAT: return 4;
                    case DXGI_FORMAT_R32G32_FLOAT: return 8;
                    case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
                    case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
                    case DXGI_FORMAT_R32_SINT: return 4;
                    case DXGI_FORMAT_R32G32_SINT: return 8;
                    case DXGI_FORMAT_R32G32B32_SINT: return 12;
                    case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
                    case DXGI_FORMAT_R32_UINT: return 4;
                    case DXGI_FORMAT_R32G32_UINT: return 8;
                    case DXGI_FORMAT_R32G32B32_UINT: return 12;
                    case DXGI_FORMAT_R32G32B32A32_UINT: return 16;
                    default: return 0;
                }
            }

            u32 PushBufferLayout(DXGI_FORMAT format, const std::string& name, RZBufferLayout& layout) /* PipelineInfo */    // I3DXXXX* DrawTypeToDX12(Razix::Gfx::DrawType type)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // TODO: Add buffer layout for all supported types
                switch (format) {
                    case DXGI_FORMAT_R8_SINT:
                        layout.push<int>(name);
                        break;
                    case DXGI_FORMAT_R32_FLOAT:
                        layout.push<f32>(name);
                        break;
                    case DXGI_FORMAT_R32G32_FLOAT:
                        layout.push<glm::vec2>(name);
                        break;
                    case DXGI_FORMAT_R32G32B32_FLOAT:
                        layout.push<glm::vec3>(name);
                        break;
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                        layout.push<glm::vec4>(name);
                        break;
                    default:
                        RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                        return 0;
                }
                return 0;
            }

            Razix::Gfx::DescriptorType DXToEngineDescriptorType(D3D_SHADER_INPUT_TYPE inputType)
            {
                switch (inputType) {
                    case D3D_SIT_CBUFFER:
                        return DescriptorType::kUniformBuffer;
                        break;
                    //case D3D_SIT_TBUFFER:
                    //    break;
                    case D3D_SIT_TEXTURE:
                        return DescriptorType::kTexture;
                        break;
                    case D3D_SIT_SAMPLER:
                        return DescriptorType::kSampler;
                        break;
                    case D3D_SIT_UAV_RWTYPED:
                        return DescriptorType::kRWTyped;
                        break;
                    case D3D_SIT_STRUCTURED:
                        return DescriptorType::kStructured;
                        break;
                    case D3D_SIT_UAV_RWSTRUCTURED:
                        return DescriptorType::kRWStructured;
                        break;
                    case D3D_SIT_BYTEADDRESS:
                        return DescriptorType::kByteAddress;
                        break;
                    case D3D_SIT_UAV_RWBYTEADDRESS:
                        return DescriptorType::kRWByteAddress;
                        break;
                    case D3D_SIT_UAV_APPEND_STRUCTURED:
                        return DescriptorType::kAppendStructured;
                        break;
                    case D3D_SIT_UAV_CONSUME_STRUCTURED:
                        return DescriptorType::kConsumeStructured;
                        break;
                    case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                        return DescriptorType::kRWStructuredCounter;
                        break;
                    case D3D_SIT_RTACCELERATIONSTRUCTURE:
                        return DescriptorType::kRTAccelerationStructure;
                        break;
                    default:
                        return DescriptorType::kNone;
                        break;
                }
            }

            // Pipeline Info

            D3D12_PRIMITIVE_TOPOLOGY_TYPE DrawTypeToDX12(Razix::Gfx::DrawType type)
            {
                switch (type) {
                    case Razix::Gfx::DrawType::Point:
                        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
                        break;
                    case Razix::Gfx::DrawType::Triangle:
                        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                        break;
                    case Razix::Gfx::DrawType::Line:
                        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
                        break;
                    default:
                        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                        break;
                }
            }

            D3D12_PRIMITIVE_TOPOLOGY DrawTypeToDX12Topology(Razix::Gfx::DrawType type)
            {
                switch (type) {
                    case Razix::Gfx::DrawType::Point:
                        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                        break;
                    case Razix::Gfx::DrawType::Triangle:
                        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                        break;
                    case Razix::Gfx::DrawType::Line:
                        return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                        break;
                    default:
                        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                        break;
                }
            }

            D3D12_FILL_MODE PolygoneModeToDX12(Razix::Gfx::PolygonMode polygonMode)
            {
                switch (polygonMode) {
                    case Razix::Gfx::PolygonMode::Fill:
                        return D3D12_FILL_MODE_SOLID;
                        break;
                    case Razix::Gfx::PolygonMode::Line:
                        return D3D12_FILL_MODE_WIREFRAME;
                        break;
                    case Razix::Gfx::PolygonMode::Point:
                        RAZIX_CORE_ERROR("[DX12] Point Fill Mode is not supported in DX12... defaulting to Fill solid");
                        return D3D12_FILL_MODE_SOLID;
                        break;
                    default:
                        return D3D12_FILL_MODE_SOLID;
                        break;
                }
            }

            D3D12_CULL_MODE CullModeToDX12(Razix::Gfx::CullMode cullMode)
            {
                switch (cullMode) {
                    case Razix::Gfx::CullMode::Back:
                        return D3D12_CULL_MODE_BACK;
                        break;
                    case Razix::Gfx::CullMode::Front:
                        return D3D12_CULL_MODE_FRONT;
                        break;
                    case Razix::Gfx::CullMode::FrontBack:
                        RAZIX_CORE_ERROR("[DX12] FrontBack Polygon Mode is not supported in DX12... defaulting to back face culling");
                        return D3D12_CULL_MODE_BACK;
                        break;
                    case Razix::Gfx::CullMode::None:
                        return D3D12_CULL_MODE_NONE;
                        break;
                    default:
                        break;
                }
            }

        }    // namespace DX12Utilities
    }        // namespace Gfx
}    // namespace Razix

#endif