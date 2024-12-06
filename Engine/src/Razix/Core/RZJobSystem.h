#pragma once

namespace Razix {
    namespace JobSystem {

        // https://www.rismosch.com/article?id=building-a-job-system

        // [Source]: https://github.com/turanszkij/JobSystem

        struct JobDispatchArgs
        {
            u32   jobIndex;
            u32   groupID;
            u32   groupIndex;           // group index relative to dispatch (like SV_GroupID in HLSL)
            bool  isFirstJobInGroup;    // is the current job the first one in the group?
            bool  isLastJobInGroup;     // is the current job the last one in the group?
            void* sharedmemory;
        };

        void OnInit();

        u32 GetThreadCount();

        struct Context
        {
            std::atomic<u32> counter{0};
        };

        // Add a job to execute asynchronously. Any idle thread will execute this job.
        void Execute(Context& ctx, const std::function<void(JobDispatchArgs)>& task);

        // Divide a job onto multiple jobs and execute in parallel.
        //	jobCount	: how many jobs to generate for this task.
        //	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
        //	func		: receives a JobDispatchArgs as parameter
        void Dispatch(Context& ctx, u32 jobCount, u32 groupSize, const std::function<void(JobDispatchArgs)>& task, sz sharedmemory_size = 0);

        u32 DispatchGroupCount(u32 jobCount, u32 groupSize);

        // Check if any threads are working currently or not
        bool IsBusy(const Context& ctx);

        // Wait until all threads become idle
        void Wait(const Context& ctx);
    }    // namespace JobSystem
}    // namespace Razix
