// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGPUProfiler.h"

namespace Razix {
    namespace Graphics {

        static u32 initial_frames_paused = 15;

        void RZGPUProfiler::Init(Memory::IRZAllocator* allocator, u32 max_frames, u32 max_queries_per_frame)
        {
            m_Allocator          = allocator;
            m_MaxFrames          = max_frames;
            m_MaxQueriesPerFrame = max_queries_per_frame;

            m_Timestamps = (GPUTimeQuery*) m_Allocator->allocate(sizeof(GPUTimeQuery) * max_frames * max_queries_per_frame, 1);

            m_PerFrameActive = (u16*) m_Allocator->allocate(sizeof(u16) * max_frames, 1);
            memset(m_PerFrameActive, 0, sizeof(u16) * max_frames);

            m_MaxDuration  = 16.667f;
            m_CurrentFrame = 0;
            m_MinTime = m_MaxTime = m_AverageTime = 0.0f;
            m_IsPaused                            = false;
            m_PipelineStats                       = nullptr;

            // Allocate GPUTimeQueriesManager memory
            m_TimeQueriesManager = (GPUTimeQueriesManager*) m_Allocator->allocate(sizeof(GPUTimeQueriesManager), 1);
            m_TimeQueriesManager->init(m_TimeQueries, allocator, gpu_time_queries_per_frame, 1, RAZIX_MAX_FRAMES);
        }

        void RZGPUProfiler::Shutdown()
        {
            m_Allocator->deallocate(m_Timestamps);
            m_Allocator->deallocate(m_PerFrameActive);
        }

        void RZGPUProfiler::update()
        {
            if (initial_frames_paused) {
                --initial_frames_paused;
                return;
            }

            // Collect TimeStamps from GPU
        }

        static f32 s_framebuffer_pixel_count = 0.f;

        // GPUTimeQueriesManager //////////////////////////////////////////////////
         
        void GPUTimeQueriesManager::init(GpuTimeQueryTree* time_queries, Memory::IRZAllocator* allocator, u16 queries_per_thread, u16 num_threads, u16 max_frames)
        {
            allocator = allocator;

        }

    }    // namespace Graphics
}    // namespace Razix