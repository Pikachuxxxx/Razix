#pragma once

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        /* D3D12 API Usage
                CPU	                                GPU
        Signal	ID3D12Fence::Signal	                ID3D12CommandQueue::Signal
        Wait	ID3D12Fence::SetEventOnCompletion,  ID3D12CommandQueue::Wait
                WaitForSingleObject	
        */

        /**
         * DirectX12 fence for managing CPU-GPU & GPU-GPU synchronization
         */
        class DX12Fence final
        {
        public:
            /**
             * Creates a Fence with the given signaled state
             * 
             * @param isSignalled Whether or not to create a fence in a signaled state or not
             */
            DX12Fence(bool isSignalled = false);
            ~DX12Fence();

            void wait(u64 fenceValue);
            u64  signal(ID3D12CommandQueue* commandQueue);
            bool isComplete(u64 fenceValue);
            void flush(ID3D12CommandQueue* commandQueue);

        private:
            ID3D12Fence* m_Fence      = nullptr; /* DX12 Fence handle  */
            HANDLE       m_FenceEvent = NULL;    /* Fence event to signal when the fence reaches a certain value */
            u64          m_FenceValue = 0;       /* The value to signal the fence with when the GPU queue has finished processing any commands */
        };
    }    // namespace Gfx

}    // namespace Razix
#endif
