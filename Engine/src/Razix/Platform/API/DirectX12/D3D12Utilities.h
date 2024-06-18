#pragma once

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

    #define CHECK_HRESULT(x) Razix::Graphics::D3D12Utilities::HRESULTCheckErrorStatus(x, __func__, __FILE__, __LINE__)

namespace Razix {
    namespace Graphics {
        namespace D3D12Utilities {

    #define D3D_SAFE_RELEASE(x) \
        if (x) {                \
            x->Release();       \
            x = nullptr;        \
        }

    #ifndef RAZIX_DISTRIBUTION

        #define D3D12_TAG_OBJECT(name, handle) handle->SetName(L##name);
    #else

        #define D3D12_TAG_OBJECT(name, handle)

    #endif
            /**
             * Transition the D3D12 resource from on state to another
             */
            void TransitionResource(ID3D12GraphicsCommandList2* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

            /**
             * Function to get the offset CPU descriptor handle pointer from the start of the heap 
             */
            void GetCPUDescriptorOffsetHandle(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, INT offsetInDescriptors, UINT descriptorIncrementSize);

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

        }    // namespace D3D12Utilities
    }        // namespace Graphics
}    // namespace Razix

#endif