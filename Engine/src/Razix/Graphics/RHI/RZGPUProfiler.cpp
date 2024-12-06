// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGPUProfiler.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RHI/API/RZSwapchain.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
#endif

#include <imgui/imgui.h>

namespace Razix {
    namespace Graphics {

        static u32 initial_frames_paused     = 15;
        static f32 s_framebuffer_pixel_count = 0.f;

        void RZGPUProfiler::Init(Memory::IRZAllocator* allocator, u32 max_frames, u32 max_queries_per_frame)
        {
            m_Allocator          = allocator;
            m_MaxFrames          = max_frames;
            m_MaxQueriesPerFrame = max_queries_per_frame;

            m_MaxDuration  = 16.667f;
            m_CurrentFrame = 0;
            m_MinTime = m_MaxTime = m_AverageTime = 0.0f;
            m_IsPaused                            = false;
            m_PipelineStats                       = (GpuPipelineStatistics*) m_Allocator->allocate(sizeof(GpuPipelineStatistics), 1);
            m_PipelineStats->reset();
        }

        void RZGPUProfiler::Shutdown()
        {
            m_Allocator->deallocate(m_PipelineStats);
            m_Allocator->deallocate(m_Timestamps);
            m_Allocator->deallocate(m_PerFrameActive);
        }

        void RZGPUProfiler::update()
        {
            std::vector<uint64_t> pipelineStats;
            pipelineStats.resize(GpuPipelineStatistics::Count);
            vkGetQueryPoolResults(VKDevice::Get().getDevice(), VKDevice::Get().getPipelineStatsQueryPool(), 0, 1, GpuPipelineStatistics::Count * sizeof(u64), pipelineStats.data(), sizeof(u64), VK_QUERY_RESULT_64_BIT);

            for (u32 i = 0; i < GpuPipelineStatistics::Count; ++i) {
                m_PipelineStats->statistics[i] += pipelineStats[i];
            }

            s_framebuffer_pixel_count = f32(RHI::GetSwapchain()->getWidth() * RHI::GetSwapchain()->getHeight());
        }

        void RZGPUProfiler::onImGuiDraw()
        {
#if 0
            if (ImGui::Begin("GPU Profiler")) {
                static const char* stat_unit_names[]       = {"Normal", "Kilo", "Mega"};
                static const char* stat_units[]            = {"", "K", "M"};
                static const f32   stat_unit_multipliers[] = {1.0f, 1000.f, 1000000.f};

                static int stat_unit_index      = 1;
                const f32  stat_unit_multiplier = stat_unit_multipliers[stat_unit_index];
                cstr       stat_unit_name       = stat_units[stat_unit_index];
                f32        stat_values[GpuPipelineStatistics::Count];
                for (u32 i = 0; i < GpuPipelineStatistics::Count; ++i) {
                    stat_values[i] = m_PipelineStats->statistics[i] / stat_unit_multiplier;
                }
                m_PipelineStats->reset();
                ImGui::Text("Vertices %f%s, Primitives %f%s", stat_values[GpuPipelineStatistics::VerticesCount], stat_unit_name, stat_values[GpuPipelineStatistics::PrimitiveCount], stat_unit_name);

                ImGui::Text("Clipping: Invocations %f%s, Visible Primitives %f%s, Visible Perc %3.1f", stat_values[GpuPipelineStatistics::ClippingInvocations], stat_unit_name, stat_values[GpuPipelineStatistics::ClippingPrimitives], stat_unit_name, stat_values[GpuPipelineStatistics::ClippingPrimitives] / stat_values[GpuPipelineStatistics::ClippingInvocations] * 100.0f, stat_unit_name);

                ImGui::Text("Invocations: Vertex Shaders %f%s, Fragment Shaders %f%s, Compute Shaders %f%s", stat_values[GpuPipelineStatistics::VertexShaderInvocations], stat_unit_name, stat_values[GpuPipelineStatistics::FragmentShaderInvocations], stat_unit_name, stat_values[GpuPipelineStatistics::ComputeShaderInvocations], stat_unit_name);

                ImGui::Text("Invocations divided by number of full screen quad pixels.");
                ImGui::Text("Vertex %f, Fragment %f, Compute %f", stat_values[GpuPipelineStatistics::VertexShaderInvocations] * stat_unit_multiplier / s_framebuffer_pixel_count, stat_values[GpuPipelineStatistics::FragmentShaderInvocations] * stat_unit_multiplier / s_framebuffer_pixel_count, stat_values[GpuPipelineStatistics::ComputeShaderInvocations] * stat_unit_multiplier / s_framebuffer_pixel_count);

                ImGui::Combo("Stat Units", &stat_unit_index, stat_unit_names, IM_ARRAYSIZE(stat_unit_names));
            }
            ImGui::End();
#endif
        }

        // GPUTimeQueriesManager //////////////////////////////////////////////////

        void GPUTimeQueriesManager::init(GpuTimeQueryTree* time_queries, Memory::IRZAllocator* allocator, u16 queries_per_thread, u16 num_threads, u16 max_frames)
        {
            allocator     = allocator;
            m_TimeQueries = time_queries;

            num_threads        = num_threads;
            queries_per_thread = queries_per_thread;
            queries_per_frame  = queries_per_thread * num_threads;

            const u32 total_time_queries = queries_per_frame * max_frames;
            const sz  allocated_size     = sizeof(GPUTimeQuery) * total_time_queries;
            u8*       memory             = (u8*) allocator->allocate(allocated_size, 1);

            timestamps = (GPUTimeQuery*) memory;
            memset(timestamps, 0, sizeof(GPUTimeQuery) * total_time_queries);

            const u32 num_pools = num_threads * max_frames;
            query_trees.resize(num_pools);

            for (u32 i = 0; i < num_pools; ++i) {
                GpuTimeQueryTree& query_tree = query_trees[i];
                query_tree.set_queries(&timestamps[i * queries_per_thread], queries_per_thread);
            }

            reset();
        }

        void GPUTimeQueriesManager::shutdown()
        {
            query_trees.clear();

            allocator->deallocate(timestamps);
        }

        void GPUTimeQueriesManager::reset()
        {
            current_frame_resolved = false;
        }

        u32 GPUTimeQueriesManager::resolve(u32 current_frame, GPUTimeQuery* timestamps_to_fill)
        {
            // For each pool
            u32 copied_timestamps = 0;
            for (u32 t = 0; t < num_threads; ++t) {
                const u32         pool_index = (num_threads * current_frame) + t;
                GpuTimeQueryTree* time_query = m_TimeQueries;
                if (time_query && time_query->allocated_time_query) {
                    memcpy(timestamps_to_fill + copied_timestamps, &timestamps[pool_index * queries_per_thread], sizeof(GPUTimeQuery) * time_query->allocated_time_query);
                    copied_timestamps += time_query->allocated_time_query;
                }
            }
            return copied_timestamps;
        }

        // GpuTimeQueryTree ///////////////////////////////////////////////////////

        void GpuTimeQueryTree::reset()
        {
            current_time_query   = 0;
            allocated_time_query = 0;
            depth                = 0;
        }

        void GpuTimeQueryTree::set_queries(GPUTimeQuery* time_queries, u32 count)
        {
            for (size_t i = 0; i < count; i++)
                this->time_queries.push_back(time_queries[i]);

            reset();
        }

        Razix::Graphics::GPUTimeQuery* GpuTimeQueryTree::push(cstr name)
        {
            GPUTimeQuery& time_query     = time_queries[allocated_time_query];
            time_query.start_query_index = allocated_time_query * 2;
            time_query.end_query_index   = time_query.start_query_index + 1;
            time_query.depth             = depth++;
            time_query.name              = name;
            time_query.parent_index      = current_time_query;

            current_time_query = allocated_time_query;
            ++allocated_time_query;

            return &time_query;
        }

        Razix::Graphics::GPUTimeQuery* GpuTimeQueryTree::pop()
        {
            GPUTimeQuery& time_query = time_queries[current_time_query];
            current_time_query       = time_query.parent_index;

            depth--;

            return &time_query;
        }

    }    // namespace Graphics
}    // namespace Razix