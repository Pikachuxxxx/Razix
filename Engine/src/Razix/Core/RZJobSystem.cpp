// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZJobSystem.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <thread>

#ifdef RAZIX_PLATFORM_WINDOWS
    #define NOMINMAX
    #include <Windows.h>
    #include <comdef.h>
#endif

namespace Razix {

    // Fixed size very simple thread safe ring buffer
    template<typename T, sz capacity>
    class ThreadSafeRingBuffer
    {
    public:
        // Push an item to the end if there is free space
        //	Returns true if successful
        //	Returns false if there is not enough space
        inline bool push_back(const T& item)
        {
            RAZIX_PROFILE_FUNCTION();
            bool result = false;
            lock.lock();
            sz next = (head + 1) % capacity;
            if (next != tail) {
                data[head] = item;
                head       = next;
                result     = true;
            }
            lock.unlock();
            return result;
        }

        // Get an item if there are any
        //	Returns true if successful
        //	Returns false if there are no items
        inline bool pop_front(T& item)
        {
            RAZIX_PROFILE_FUNCTION();
            bool result = false;
            lock.lock();
            if (tail != head) {
                item   = data[tail];
                tail   = (tail + 1) % capacity;
                result = true;
            }
            lock.unlock();
            return result;
        }

    private:
        T          data[capacity];
        sz     head = 0;
        sz     tail = 0;
        std::mutex lock;
    };

    struct Job
    {
        Context*                             ctx;
        std::function<void(JobDispatchArgs)> task;
        u32                             groupID;
        u32                             groupJobOffset;
        u32                             groupJobEnd;
        u32                             sharedmemory_size;
    };

    u32                       numThreads = 0;
    ThreadSafeRingBuffer<Job, 256> jobQueue;
    std::condition_variable        wakeCondition;
    std::mutex                     wakeMutex;

    inline bool work()
    {
        RAZIX_PROFILE_FUNCTION();
        Job job;
        if (jobQueue.pop_front(job)) {
            JobDispatchArgs args;
            args.groupID = job.groupID;
            if (job.sharedmemory_size > 0) {
                args.sharedmemory = alloca(job.sharedmemory_size);
            } else {
                args.sharedmemory = nullptr;
            }

            for (u32 i = job.groupJobOffset; i < job.groupJobEnd; ++i) {
                RAZIX_PROFILE_SCOPE("Group Loop");
                args.jobIndex          = i;
                args.groupIndex        = i - job.groupJobOffset;
                args.isFirstJobInGroup = (i == job.groupJobOffset);
                args.isLastJobInGroup  = (i == job.groupJobEnd - 1);
                job.task(args);
            }

            job.ctx->counter.fetch_sub(1);
            return true;
        }
        return false;
    }

    void OnInit()
    {
        // Retrieve the number of hardware threads in this System:
        auto numCores = std::thread::hardware_concurrency();

        // Calculate the actual number of worker threads we want:
        numThreads = glm::max(1U, numCores - 1);

        for (u32 threadID = 0; threadID < numThreads; ++threadID) {
            std::thread worker([threadID] {
                std::stringstream ss;
                ss << "JobSystem_" << threadID;
                RAZIX_PROFILE_SETTHREADNAME(ss.str().c_str());

                while (true) {
                    if (!work()) {
                        // no job, put thread to sleep
                        std::unique_lock<std::mutex> lock(wakeMutex);
                        wakeCondition.wait(lock);
                    }
                }
            });

            // Do Windows-specific thread setup:
            HANDLE handle = (HANDLE) worker.native_handle();

            // Put each thread on to dedicated core
            DWORD_PTR affinityMask    = 1ull << threadID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            RAZIX_ASSERT(affinity_result > 0, "");

            // Increase thread priority:
            BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
            RAZIX_ASSERT(priority_result != 0, "");

            // Name the thread:
            std::wstringstream wss;
            wss << "JobSystem_" << threadID;
            HRESULT hr = SetThreadDescription(handle, wss.str().c_str());

            RAZIX_ASSERT(SUCCEEDED(hr), "");

            worker.detach();
        }

        RAZIX_CORE_INFO("Initialised JobSystem with [{0} cores] [{1} threads]", numCores, numThreads);
    }

    u32 GetThreadCount()
    {
        return numThreads;
    }

    void Execute(Context& ctx, const std::function<void(JobDispatchArgs)>& task)
    {
        // Context state is updated:
        ctx.counter.fetch_add(1);

        Job job;
        job.ctx               = &ctx;
        job.task              = task;
        job.groupID           = 0;
        job.groupJobOffset    = 0;
        job.groupJobEnd       = 1;
        job.sharedmemory_size = 0;

        // Try to push a new job until it is pushed successfully:
        while (!jobQueue.push_back(job)) {
            wakeCondition.notify_all();
            work();
        }

        // Wake any one thread that might be sleeping:
        wakeCondition.notify_one();
    }

    void Dispatch(Context& ctx, u32 jobCount, u32 groupSize, const std::function<void(JobDispatchArgs)>& task, sz sharedmemory_size)
    {
        RAZIX_PROFILE_FUNCTION();
        if (jobCount == 0 || groupSize == 0) {
            return;
        }

        const u32 groupCount = DispatchGroupCount(jobCount, groupSize);

        // Context state is updated:
        ctx.counter.fetch_add(groupCount);

        Job job;
        job.ctx               = &ctx;
        job.task              = task;
        job.sharedmemory_size = (u32) sharedmemory_size;

        for (u32 groupID = 0; groupID < groupCount; ++groupID) {
            // For each group, generate one real job:
            job.groupID        = groupID;
            job.groupJobOffset = groupID * groupSize;
            job.groupJobEnd    = std::min(job.groupJobOffset + groupSize, jobCount);

            // Try to push a new job until it is pushed successfully:
            while (!jobQueue.push_back(job)) {
                wakeCondition.notify_all();
                work();
            }
        }
    }

    u32 DispatchGroupCount(u32 jobCount, u32 groupSize)
    {
        // Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
        return (jobCount + groupSize - 1) / groupSize;
    }

    bool IsBusy(const Context& ctx)
    {
        // Whenever the main thread label is not reached by the workers, it indicates that some worker is still alive
        return ctx.counter.load() > 0;
    }

    void Wait(const Context& ctx)
    {
        wakeCondition.notify_all();
        while (IsBusy(ctx)) { work(); }
    }
}    // namespace Razix
