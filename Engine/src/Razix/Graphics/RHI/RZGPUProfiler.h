#pragma once

#ifdef RAZIX_RENDER_API_VULKAN
    #include <vulkan/vulkan.h>
#endif

namespace Razix {
    namespace Graphics {

        constexpr u32 gpu_time_queries_per_frame = 32;

        struct GPUTimeStamp
        {
            u32 start;
            u32 end;
            f32 delta;

            u32 depth;
            u32 parentIdx;

            cstr name;
        };

        struct GPUTimeQuery
        {
            f32 elapsed_ms;

            u16 start_query_index;    // Used to write timestamp in the query pool
            u16 end_query_index;      // Used to write timestamp in the query pool

            u16 parent_index;
            u16 depth;

            u32 color;
            u32 frame_index;

            cstr name;
        };

        // Query tree used mainly per thread-frame to retrieve time data.
        struct GpuTimeQueryTree
        {
            void reset();
            void set_queries(GPUTimeQuery* time_queries, u32 count);

            GPUTimeQuery* push(cstr name);
            GPUTimeQuery* pop();

            std::vector<GPUTimeQuery> time_queries;    // Allocated externally

            u16 current_time_query   = 0;
            u16 allocated_time_query = 0;
            u16 depth                = 0;
        };

        struct GpuPipelineStatistics
        {
            enum Stats : u8
            {
                VerticesCount,
                PrimitiveCount,
                VertexShaderInvocations,
                ClippingInvocations,
                ClippingPrimitives,
                FragmentShaderInvocations,
                ComputeShaderInvocations,
                Count
            };

            void reset();

            u64 statistics[Count];
        };

        struct GPUTimeQueriesManager
        {
            void init(GpuTimeQueryTree* time_queries, Memory::IRZAllocator* allocator, u16 queries_per_thread, u16 num_threads, u16 max_frames);
            void shutdown();

            void reset();
            u32  resolve(u32 current_frame, GPUTimeQuery* timestamps_to_fill);    // Returns the total queries for this frame.

            std::vector<GpuTimeQueryTree> query_trees;

            Memory::IRZAllocator* allocator  = nullptr;
            GPUTimeQuery*         timestamps = nullptr;

            GpuPipelineStatistics frame_pipeline_statistics;    // Per frame statistics as sum of per-frame ones.

            u32 queries_per_thread = 0;
            u32 queries_per_frame  = 0;
            u32 num_threads        = 0;

            bool current_frame_resolved = false;    // Used to query the GPU only once per frame if get_gpu_timestamps is called more than once per frame.
        };

        class RAZIX_API RZGPUProfiler
        {
        public:
            void Init(Memory::IRZAllocator* allocator, u32 max_frames, u32 max_queries_per_frame);
            void Shutdown();

            void update();

            void onImGuiDraw();

        private:
            Memory::IRZAllocator*  m_Allocator;
            GPUTimeQuery*          m_Timestamps;    // Per frame timestamps collected from the profiler.
            u16*                   m_PerFrameActive;
            GpuTimeQueryTree*      m_TimeQueries;
            GPUTimeQueriesManager* m_TimeQueriesManager;
            GpuPipelineStatistics* m_PipelineStats;    // Per frame collected pipeline statistics.
            u32                    m_MaxFrames;
            u32                    m_MaxQueriesPerFrame;
            u32                    m_CurrentFrame;
            f32                    m_MaxTime;
            f32                    m_MinTime;
            f32                    m_AverageTime;
            f32                    m_MaxDuration;
            bool                   m_IsPaused;
        };
    }    // namespace Graphics
}    // namespace Razix
