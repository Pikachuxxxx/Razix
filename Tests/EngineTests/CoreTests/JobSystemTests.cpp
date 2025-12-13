#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/thread.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <cstring>

namespace Razix {
namespace {
    using Clock = std::chrono::steady_clock;

    struct JobWrapper
    {
        rz_job      job;
        rz_job_cold cold;
    };

    // Clears the job struct and wires in the payload/function pointers.
    template<typename Payload>
    void PrepareJob(JobWrapper& wrapper, Payload* payload, void (*func)(rz_job*))
    {
        std::memset(&wrapper, 0, sizeof(wrapper));
        wrapper.job.hot.pFunc     = func;
        wrapper.job.hot.pUserData = payload;
        wrapper.job.pCold         = &wrapper.cold;
    }

    struct HitTaskPayload
    {
        JobWrapper* jobWrapper;
        u32*        flag;
    };

    static void HitTask(rz_job* userData)
    {
        auto* payload = reinterpret_cast<HitTaskPayload*>(userData);
        *payload->flag = 1u;
        RAZIX_UNUSED(payload->jobWrapper);
    }

    struct BusyWorkTaskPayload
    {
        JobWrapper* jobWrapper;
        u32*        doneFlag;
        u32         busyMicros;
    };

    static void BusyWorkTask(rz_job* userData)
    {
        auto* payload = reinterpret_cast<BusyWorkTaskPayload*>(userData);
        rz_thread_busy_wait_micro(payload->busyMicros);
        *payload->doneFlag = 1u;
        RAZIX_UNUSED(payload->jobWrapper);
    }

    struct FileIoTaskPayload
    {
        JobWrapper* jobWrapper;
        char        path[128];
        char        contents[64];
        u32*        doneFlag;
        u32*        bytesReadSlot;
    };

    static void FileIoTask(rz_job* userData)
    {
        auto* payload = reinterpret_cast<FileIoTaskPayload*>(userData);

        std::FILE* out = std::fopen(payload->path, "w");
        if (out != nullptr) {
            std::fwrite(payload->contents, 1, std::strlen(payload->contents), out);
            std::fclose(out);
        }

        std::FILE* in = std::fopen(payload->path, "r");
        if (in != nullptr) {
            char   buffer[64];
            size_t readBytes = std::fread(buffer, 1, sizeof(buffer), in);
            *payload->bytesReadSlot = static_cast<u32>(readBytes);
            std::fclose(in);
        }

        std::remove(payload->path);
        *payload->doneFlag = 1u;
        RAZIX_UNUSED(payload->jobWrapper);
    }

    struct ChildSpawnTaskPayload
    {
        JobWrapper* jobWrapper;
        u32*        doneFlag;
    };

    static void ChildSpawnTask(rz_job* userData)
    {
        auto* payload = reinterpret_cast<ChildSpawnTaskPayload*>(userData);
        *payload->doneFlag = 1u;
        RAZIX_UNUSED(payload->jobWrapper);
    }

    struct ParentSpawnTaskPayload
    {
        JobWrapper*                           jobWrapper;
        RZDynamicArray<ChildSpawnTaskPayload>* children;
        u32*                                  parentRanFlag;
    };

    static void ParentSpawnTask(rz_job* userData)
    {
        auto* payload = reinterpret_cast<ParentSpawnTaskPayload*>(userData);
        *payload->parentRanFlag = 1u;
        for (u32 i = 0; i < payload->children->size(); ++i) {
            ChildSpawnTaskPayload& childPayload = (*payload->children)[i];
            rz_job_system_worker_spawn_job(&childPayload.jobWrapper->job);
        }
        RAZIX_UNUSED(payload->jobWrapper);
    }

    class RZJobSystemFixture : public ::testing::Test
    {
    protected:
        static constexpr u32 kWorkerCount = 4u;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
            rz_job_system_startup(kWorkerCount);
        }

        void TearDown() override
        {
            rz_job_system_shutdown();
            Razix::Debug::RZLog::Shutdown();
        }
    };
}    // namespace

TEST_F(RZJobSystemFixture, ExecutesSingleJob)
{
    u32 hitFlag = 0u;

    JobWrapper      wrapper{};
    HitTaskPayload payload{&wrapper, &hitFlag};
    PrepareJob(wrapper, &payload, HitTask);

    rz_job_system_submit_job(&wrapper.job);
    rz_job_system_wait_for_all();

    EXPECT_EQ(hitFlag, 1u);
}

TEST_F(RZJobSystemFixture, RunsMultipleIndependentJobs)
{
    constexpr u32 jobCount = 12u;

    RZDynamicArray<u32>           hitFlags;
    RZDynamicArray<JobWrapper>    wrappers;
    RZDynamicArray<HitTaskPayload> payloads;
    hitFlags.resize(jobCount);
    wrappers.resize(jobCount);
    payloads.resize(jobCount);

    for (u32 i = 0; i < jobCount; ++i) {
        hitFlags[i]          = 0u;
        payloads[i].jobWrapper = &wrappers[i];
        payloads[i].flag       = &hitFlags[i];
        PrepareJob(wrappers[i], &payloads[i], HitTask);
        rz_job_system_submit_job(&wrappers[i].job);
    }

    rz_job_system_wait_for_all();

    u32 totalHits = 0u;
    for (u32 i = 0; i < jobCount; ++i)
        totalHits += hitFlags[i];

    EXPECT_EQ(totalHits, jobCount);
}

TEST_F(RZJobSystemFixture, WorkerSpawnedJobsComplete)
{
    constexpr u32 childCount = 6u;

    RZDynamicArray<u32>                childFlags;
    RZDynamicArray<JobWrapper>         childWrappers;
    RZDynamicArray<ChildSpawnTaskPayload> childPayloads;
    childFlags.resize(childCount);
    childWrappers.resize(childCount);
    childPayloads.resize(childCount);

    for (u32 i = 0; i < childCount; ++i) {
        childFlags[i]              = 0u;
        childPayloads[i].jobWrapper = &childWrappers[i];
        childPayloads[i].doneFlag   = &childFlags[i];
        PrepareJob(childWrappers[i], &childPayloads[i], ChildSpawnTask);
    }

    u32                    parentFlag    = 0u;
    JobWrapper             parentWrapper{};
    ParentSpawnTaskPayload parentPayload{&parentWrapper, &childPayloads, &parentFlag};
    PrepareJob(parentWrapper, &parentPayload, ParentSpawnTask);

    rz_job_system_submit_job(&parentWrapper.job);

    rz_job_system_wait_for_all();

    u32 completedChildren = 0u;
    for (u32 i = 0; i < childCount; ++i)
        completedChildren += childFlags[i];

    EXPECT_EQ(parentFlag, 1u);
    EXPECT_EQ(completedChildren, childCount);
}

TEST_F(RZJobSystemFixture, ParallelWorkIsFasterThanSerialWithinSlack)
{
    constexpr u32 jobCount        = 16u;
    constexpr u32 busyMicrosPerJob = 1500u;

    // Serial baseline
    auto serialStart = Clock::now();
    for (u32 i = 0; i < jobCount; ++i)
        rz_thread_busy_wait_micro(busyMicrosPerJob);
    auto serialElapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - serialStart).count();

    RZDynamicArray<u32>                doneFlags;
    RZDynamicArray<JobWrapper>         wrappers;
    RZDynamicArray<BusyWorkTaskPayload> payloads;
    doneFlags.resize(jobCount);
    wrappers.resize(jobCount);
    payloads.resize(jobCount);

    const auto jobStart = Clock::now();
    for (u32 i = 0; i < jobCount; ++i) {
        doneFlags[i]              = 0u;
        payloads[i].jobWrapper    = &wrappers[i];
        payloads[i].doneFlag      = &doneFlags[i];
        payloads[i].busyMicros    = busyMicrosPerJob;
        PrepareJob(wrappers[i], &payloads[i], BusyWorkTask);
        rz_job_system_submit_job(&wrappers[i].job);
    }

    rz_job_system_wait_for_all();

    u32 finished = 0u;
    for (u32 i = 0; i < jobCount; ++i)
        finished += doneFlags[i];

    ASSERT_EQ(finished, jobCount);

    auto jobElapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - jobStart).count();

    // Allow a generous 1.8x headroom for CI variance.
    EXPECT_LT(jobElapsed, static_cast<int64_t>(serialElapsed * 18 / 10 + 5000));
}

TEST_F(RZJobSystemFixture, DistributedFileIOCreatesAndCleansUp)
{
    constexpr u32 jobCount = 5u;

    RZDynamicArray<u32>              doneFlags;
    RZDynamicArray<u32>              bytesReadSlots;
    RZDynamicArray<JobWrapper>       wrappers;
    RZDynamicArray<FileIoTaskPayload> payloads;
    doneFlags.resize(jobCount);
    bytesReadSlots.resize(jobCount);
    wrappers.resize(jobCount);
    payloads.resize(jobCount);

    for (u32 i = 0; i < jobCount; ++i) {
        doneFlags[i]        = 0u;
        bytesReadSlots[i]   = 0u;
        FileIoTaskPayload& payload = payloads[i];
        payload.jobWrapper        = &wrappers[i];
        payload.doneFlag          = &doneFlags[i];
        payload.bytesReadSlot     = &bytesReadSlots[i];

        rz_snprintf(payload.contents, sizeof(payload.contents), "job-%u-data", i);
        rz_snprintf(payload.path, sizeof(payload.path), "/tmp/rz_job_system_%llu_%u.tmp", static_cast<unsigned long long>(rz_thread_get_current_id()), i);

        PrepareJob(wrappers[i], &payload, FileIoTask);
        rz_job_system_submit_job(&wrappers[i].job);
    }

    rz_job_system_wait_for_all();

    u32 completedCount = 0u;
    u32 totalBytes     = 0u;
    for (u32 i = 0; i < jobCount; ++i) {
        completedCount += doneFlags[i];
        totalBytes     += bytesReadSlots[i];
    }

    EXPECT_EQ(completedCount, jobCount);
    EXPECT_GT(totalBytes, 0u);

    // Ensure files were cleaned up
    for (u32 i = 0; i < jobCount; ++i) {
        std::FILE* stray = std::fopen(payloads[i].path, "r");
        EXPECT_EQ(stray, nullptr);
        if (stray != nullptr) {
            std::fclose(stray);
            std::remove(payloads[i].path);
        }
    }
}

}    // namespace Razix
