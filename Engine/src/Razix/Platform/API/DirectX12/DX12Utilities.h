#pragma once

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

    #define CHECK_HRESULT(x) Razix::Gfx::DX12Utilities::HRESULTCheckErrorStatus(x, __func__, __FILE__, __LINE__)

namespace Razix {
    namespace Gfx {
        namespace DX12Utilities {

    #define D3D_SAFE_RELEASE(x) \
        if (x) {                \
            x->Release();       \
            x = nullptr;        \
        }

    #ifndef RAZIX_GOLD_MASTER

        #define D3D12_TAG_OBJECT(handle, name) handle->SetName(name);
    #else

        #define D3D12_TAG_OBJECT(handle, name)

    #endif

            typedef void(WINAPI* BeginEventOnCommandList)(ID3D12GraphicsCommandList* commandList, UINT64 color, _In_ PCSTR formatString);
            typedef void(WINAPI* EndEventOnCommandList)(ID3D12GraphicsCommandList* commandList);
            typedef void(WINAPI* SetMarkerOnCommandList)(ID3D12GraphicsCommandList* commandList, UINT64 color, _In_ PCSTR formatString);

            void LoadPIXRuntime();

            //-----------------------------------------------------------------------------------
            // HRESULT enums and their error descriptions map
            static std::unordered_map<HRESULT, std::string> ErrorDescriptions = {
                {S_OK, "Operation successful"},
                {E_ABORT, "Operation aborted"},
                {E_ACCESSDENIED, "General access denied error"},
                {E_FAIL, "Unspecified failure"},
                {E_HANDLE, "Handle that is not valid"},
                {E_INVALIDARG, "One or more arguments are not valid"},
                {E_NOINTERFACE, "No such interface supported"},
                {E_NOTIMPL, "Not implemented"},
                {E_OUTOFMEMORY, "Failed to allocate necessary memory"},
                {E_POINTER, "Pointer that is not valid"},
                {E_UNEXPECTED, "Unexpected failure"}};

            /* 
             * Error reporting for Vulkan results
             * @returns True, if any error has occurred
             */
            static bool HRESULTCheckErrorStatus(HRESULT x, cstr function, cstr file, int line)
            {
                if (x != S_OK) {
                    RAZIX_CORE_ERROR("[D3D12] HRESULT Error :: Description : {0} (by Function : {1} at Line : {2} in File : {3})", ErrorDescriptions[x], function, line, file);
                    return false;
                } else
                    return true;
            }

            //-----------------------------------------------------------------------------------
            // Markers for API Debugging in Graphics debugger Tools (PIX/RenderDoc)

            void CmdBeginLabel(ID3D12GraphicsCommandList2* commandList, const std::string& name, float4 color);
            void CmdInsertLabel(ID3D12GraphicsCommandList2* commandList, const std::string& name, float4 color);
            void CmdEndLabel(ID3D12GraphicsCommandList2* commandList);

            //-----------------------------------------------------------------------------------
            // Single Time Command Buffer utility functions
            //-----------------------------------------------------------------------------------

            /* Creates a command buffer for single time use */
            ID3D12GraphicsCommandList2* BeginSingleTimeCommandBuffer(const std::string commandUsage, float4 color);
            /* Ends the recording of the single time command buffer */
            void EndSingleTimeCommandBuffer(ID3D12GraphicsCommandList2* commandList);

            //-----------------------------------------------------------------------------------
            // Memory and Barriers related stuff
            //-----------------------------------------------------------------------------------

            /**
             * Transition the D3D12 resource from on state to another
             */
            void TransitionResource(ID3D12GraphicsCommandList2* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

            void UpdateBufferResource(ID3D12Resource** pDestinationResource, size_t bufferSize, const void* bufferData);

            D3D12_RESOURCE_STATES EngineImageLayoutToDX12(ImageLayout layout);

            //-----------------------------------------------------------------------------------
            // Texture/Image utility Functions
            //-----------------------------------------------------------------------------------

            /**
             * Function to get the offset CPU descriptor handle pointer from the start of the heap 
             */
            void GetCPUDescriptorOffsetHandle(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, INT offsetInDescriptors, UINT descriptorIncrementSize);

            //-----------------------------------------------------------------------------------
            // Enum Conversions
            //-----------------------------------------------------------------------------------

            // Vertex Input
            DXGI_FORMAT GetFormatFromComponentType(D3D_REGISTER_COMPONENT_TYPE componentType, u32 componentCount);
            u32         GetFormatSize(DXGI_FORMAT format);
            u32         PushBufferLayout(DXGI_FORMAT format, const std::string& name, RZBufferLayout& layout);

            // Descriptor
            DescriptorType DXToEngineDescriptorType(D3D_SHADER_INPUT_TYPE inputType);

            D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapTypeToDX12(DescriptorHeapType heapType);

            D3D12_SHADER_VISIBILITY ShaderStageToVisibility(ShaderStage stage);

            // PipelineInfo
            /**
             * Converts the draw type that is used to draw geometry into DX12 enum value
             * for the pipeline
             * 
             * @param type The primitive draw type, value is one of POINT, TRIANGLES and LINE
             * 
             * @returns DX12 equivalent value of primitive topology 
             */
            D3D12_PRIMITIVE_TOPOLOGY_TYPE DrawTypeToDX12(Razix::Gfx::DrawType type);
            D3D12_PRIMITIVE_TOPOLOGY      DrawTypeToDX12Topology(Razix::Gfx::DrawType type);
            D3D12_FILL_MODE               PolygoneModeToDX12(Razix::Gfx::PolygonMode polygonMode);
            D3D12_CULL_MODE               CullModeToDX12(Razix::Gfx::CullMode cullMode);
            // I3DXXXX* BlendOpToDX12(Razix::Gfx::BlendOp blendOp);
            // I3DXXXX* BlendFactorToDX12(Razix::Gfx::BlendFactor blendFactor);
            // I3DXXXX* CompareOpToDX12(Razix::Gfx::CompareOp compareOp);
            // I3DXXXX* DescriptorTypeToDX12(Razix::Gfx::DescriptorType descriptorType);
            // I3DXXXX* ShaderStageToDX12(Razix::Gfx::ShaderStage stage);

        }    // namespace DX12Utilities
    }        // namespace Gfx
}    // namespace Razix

#endif