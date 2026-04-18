#pragma once

#include "Razix/Core/std/thread.h"

#include "Razix/Gfx/RZWorld.h"

namespace Razix {
    namespace Gfx {
        
        // This is struct is handed off to RenderThread by the GameThread at end of each frame
        // It builds the RZWorld based on the scene data and render thread will read from it when data is ready
        //
        // This is the handoff struct from Scene (ECS) --> RZWorld data for RHI.
        //
        // This is owned by Engine and is used to pass data from GameThread to RenderThread, it is not used for
        // anything else and is not exposed to users of the engine, it's purely an internal struct for the 
        // engine's multi-threaded rendering architecture
        //
        // House is responsible for translating the scene data into RZWorld data
        struct WorldRingBuffer
        {
            RZWorld worldBuffers[RAZIX_MAX_FRAMES_IN_FLIGHT + 1];   // +1 for the frame being rendered while the other frames in flight are being recorded
            rz_atomic_u32 writeBufferIdx = 0;                       // index of the current buffer being used for writing, will be incremented atomically at the end of each frame recording
            rz_atomic_u32 readBufferIdx = 0;                        // index of the buffer being read by the render thread, used internally by render thread and for debugging purposes 
        };

        rz_thread_handle RenderThreadCreate();
        void RenderThreadDestroy(rz_thread_handle thread);
        
        // Main render function where RZWorldRenderer::drawFrame is called
        void RenderThreadRunRenderLoop();
    }
}    // namespace Razix
