#pragma once

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/thread.h"

#include "Razix/Gfx/RZWorld.h"
#include <Core/RZCore.h>

#define RAZIX_WORLDS_IN_FLIGHT (RAZIX_MAX_FRAMES_IN_FLIGHT + 1)

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
            RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
            rz_atomic_u64 m_RenderThreadWorldDataReadCounter = 0;

            RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
            rz_atomic_u64 m_GameThreadWorldDataWriteCounter = 0;

            RZWorld worldBuffers[RAZIX_WORLDS_IN_FLIGHT];
        };

        extern WorldRingBuffer g_RenderThreadRingBuffer;
        extern rz_atomic_u32   g_RenderThreadIsRunning;
        extern rz_atomic_u32   g_ResizePending;

        rz_thread_handle RenderThreadCreate();
        void             RenderThreadDestroy(rz_thread_handle thread);

        // Main render function where RZWorldRenderer::drawFrame is called
        // void RenderThreadRunRenderLoop();
    }    // namespace Gfx
}    // namespace Razix
