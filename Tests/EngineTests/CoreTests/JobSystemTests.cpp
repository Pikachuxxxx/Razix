#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/thread.h"

#include <gtest/gtest.h>

#include <atomic>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace Razix {
    namespace {
        using Clock = std::chrono::steady_clock;

        struct JobWrapper
        {
            rz_job      job;
            rz_job_cold cold;
        };

        struct TempFileInfo
        {
            std::string path;
            size_t      sizeBytes = 0u;
        };

        struct TempFileSet
        {
            std::string              baseDir;
            std::vector<TempFileInfo> files;
            size_t                   totalBytes = 0u;
        };

        struct TempDirGuard
        {
            std::string path;

            explicit TempDirGuard(std::string dir)
                : path(std::move(dir))
            {
            }

            ~TempDirGuard()
            {
                if (!path.empty()) {
                    std::error_code ec{};
                    std::filesystem::remove_all(path, ec);
                }
            }
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
            auto* payload  = reinterpret_cast<HitTaskPayload*>(userData);
            *payload->flag = 1u;
            RAZIX_UNUSED(payload->jobWrapper);
        }
    // rz_worker* pWorker = _rz_find_best_worker_to_submit_to_();
    // if (pWorker) {

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
                size_t readBytes        = std::fread(buffer, 1, sizeof(buffer), in);
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
            auto* payload      = reinterpret_cast<ChildSpawnTaskPayload*>(userData);
            *payload->doneFlag = 1u;
            RAZIX_UNUSED(payload->jobWrapper);
        }

        struct ParentSpawnTaskPayload
        {
            JobWrapper*                            jobWrapper;
            RZDynamicArray<ChildSpawnTaskPayload>* children;
            u32*                                   parentRanFlag;
        };

        struct FileReadJobPayload
        {
            JobWrapper*                 jobWrapper;
            const char*                 path;
            std::atomic<uint64_t>*      totalBytes;
            std::atomic<uint32_t>*      completedCount;
            std::atomic<uint32_t>*      failures;
        };

        struct MasterKickoffPayload
        {
            JobWrapper*                         jobWrapper;
            RZDynamicArray<JobWrapper>*         children;
            RZDynamicArray<FileReadJobPayload>* payloads;
        };

        // Fills files with random ASCII data up to the requested size and returns their locations.
        static TempFileSet CreateTempFilesForIoTest(u32 fileCount, size_t maxBytesPerFile)
        {
            TempFileSet result{};
            namespace fs = std::filesystem;

            const auto nowTicks = Clock::now().time_since_epoch().count();
            result.baseDir      = "/tmp/razix_job_io_" + std::to_string(nowTicks);

            std::error_code ec{};
            fs::create_directories(result.baseDir, ec);
            if (ec) {
                ADD_FAILURE() << "Unable to create temp dir: " << ec.message();
                return result;
            }

            std::mt19937_64                     rng(static_cast<unsigned>(nowTicks));
            std::uniform_int_distribution<int>  charDist(32, 126);
            std::uniform_int_distribution<size_t> sizeDist(1u, maxBytesPerFile);

            result.files.reserve(fileCount);

            for (u32 i = 0; i < fileCount; ++i) {
                TempFileInfo info{};
                info.sizeBytes = sizeDist(rng);
                info.path      = result.baseDir + "/file_" + std::to_string(i) + ".bin";

                std::vector<char> data(info.sizeBytes);
                for (size_t j = 0; j < data.size(); ++j)
                    data[j] = static_cast<char>(charDist(rng));

                std::ofstream out(info.path, std::ios::binary | std::ios::out | std::ios::trunc);
                if (!out.is_open()) {
                    ADD_FAILURE() << "Failed to open " << info.path;
                    return result;
                }
                out.write(data.data(), static_cast<std::streamsize>(data.size()));
                out.close();

                std::error_code permEc{};
                fs::permissions(info.path, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read, fs::perm_options::add, permEc);
                if (permEc) {
                    ADD_FAILURE() << "Failed to set permissions on " << info.path << ": " << permEc.message();
                    return result;
                }

                if (!fs::exists(info.path)) {
                    ADD_FAILURE() << "File missing after creation: " << info.path;
                    return result;
                }

                result.totalBytes += info.sizeBytes;
                result.files.emplace_back(std::move(info));
            }

            return result;
        }

        static size_t BlockingReadFile(const TempFileInfo& info)
        {
            size_t             totalRead = 0u;
            std::array<char, 131072> buffer{};    // 128 KB buffer
            std::FILE*         in = std::fopen(info.path.c_str(), "rb");
            if (in == nullptr)
                return 0u;

            while (true) {
                const size_t bytes = std::fread(buffer.data(), 1, buffer.size(), in);
                totalRead += bytes;
                if (bytes < buffer.size())
                    break;
            }

            std::fclose(in);
            return totalRead;
        }

        static size_t SequentialReadBytes(const std::vector<TempFileInfo>& files)
        {
            size_t total = 0u;
            for (const auto& f : files)
                total += BlockingReadFile(f);
            return total;
        }

        static void FileAsyncReadJob(rz_job* userData)
        {
            auto* payload = reinterpret_cast<FileReadJobPayload*>(userData);

            std::array<char, 131072> buffer{};
            std::FILE*               in = std::fopen(payload->path, "rb");
            if (in == nullptr) {
                payload->failures->fetch_add(1u, std::memory_order_relaxed);
                payload->completedCount->fetch_add(1u, std::memory_order_relaxed);
                return;
            }

            uint64_t localBytes = 0u;
            while (true) {
                const size_t bytes = std::fread(buffer.data(), 1, buffer.size(), in);
                localBytes += bytes;
                if (bytes < buffer.size())
                    break;
            }

            std::fclose(in);
            payload->totalBytes->fetch_add(localBytes, std::memory_order_relaxed);
            payload->completedCount->fetch_add(1u, std::memory_order_relaxed);
            RAZIX_UNUSED(payload->jobWrapper);
        }

        static void FileReadAsyncMasterKickoff(rz_job* userData)
        {
            auto* payload = reinterpret_cast<MasterKickoffPayload*>(userData);
            for (u32 i = 0; i < payload->payloads->size(); ++i)
                rz_job_system_worker_spawn_job(&(*payload->children)[i].job);
            RAZIX_UNUSED(payload->jobWrapper);
        }

        static void ParentSpawnTask(rz_job* userData)
        {
            auto* payload           = reinterpret_cast<ParentSpawnTaskPayload*>(userData);
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

        class RZJobSystemHighWorkerFixture : public ::testing::Test
        {
        protected:
            static constexpr u32 kWorkerCount = 8u;

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

        JobWrapper     wrapper{};
        HitTaskPayload payload{&wrapper, &hitFlag};
        PrepareJob(wrapper, &payload, HitTask);

        rz_job_system_submit_job(&wrapper.job);
        rz_job_system_wait_for_all();

        EXPECT_EQ(hitFlag, 1u);
    }

    TEST_F(RZJobSystemFixture, RunsMultipleIndependentJobs)
    {
        constexpr u32 jobCount = 12u;

        RZDynamicArray<u32>            hitFlags;
        RZDynamicArray<JobWrapper>     wrappers;
        RZDynamicArray<HitTaskPayload> payloads;
        hitFlags.resize(jobCount);
        wrappers.resize(jobCount);
        payloads.resize(jobCount);

        for (u32 i = 0; i < jobCount; ++i) {
            hitFlags[i]            = 0u;
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

        RZDynamicArray<u32>                   childFlags;
        RZDynamicArray<JobWrapper>            childWrappers;
        RZDynamicArray<ChildSpawnTaskPayload> childPayloads;
        childFlags.resize(childCount);
        childWrappers.resize(childCount);
        childPayloads.resize(childCount);

        for (u32 i = 0; i < childCount; ++i) {
            childFlags[i]               = 0u;
            childPayloads[i].jobWrapper = &childWrappers[i];
            childPayloads[i].doneFlag   = &childFlags[i];
            PrepareJob(childWrappers[i], &childPayloads[i], ChildSpawnTask);
        }

        u32                    parentFlag = 0u;
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
        constexpr u32 jobCount         = 16u;
        constexpr u32 busyMicrosPerJob = 1500u;

        // Serial baseline
        auto serialStart = Clock::now();
        for (u32 i = 0; i < jobCount; ++i)
            rz_thread_busy_wait_micro(busyMicrosPerJob);
        auto serialElapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - serialStart).count();

        RZDynamicArray<u32>                 doneFlags;
        RZDynamicArray<JobWrapper>          wrappers;
        RZDynamicArray<BusyWorkTaskPayload> payloads;
        doneFlags.resize(jobCount);
        wrappers.resize(jobCount);
        payloads.resize(jobCount);

        const auto jobStart = Clock::now();
        for (u32 i = 0; i < jobCount; ++i) {
            doneFlags[i]           = 0u;
            payloads[i].jobWrapper = &wrappers[i];
            payloads[i].doneFlag   = &doneFlags[i];
            payloads[i].busyMicros = busyMicrosPerJob;
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

    TEST_F(RZJobSystemFixture, MacSequentialVsSubmittedFileReads)
    {
        constexpr u32   kFileCount       = 150u;
        constexpr size_t kMaxFileSizeB   = 256u * 1024u;    // Up to 256 KB per file

        TempFileSet files = CreateTempFilesForIoTest(kFileCount, kMaxFileSizeB);
        TempDirGuard cleanup(files.baseDir);
        ASSERT_EQ(files.files.size(), kFileCount);

        const auto serialStart = Clock::now();
        const size_t serialBytesRead = SequentialReadBytes(files.files);
        const auto serialMicros      = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - serialStart).count();

        ASSERT_EQ(serialBytesRead, files.totalBytes);

        std::atomic<uint64_t> totalBytes{0u};
        std::atomic<uint32_t> completed{0u};
        std::atomic<uint32_t> failures{0u};

        RZDynamicArray<JobWrapper>         wrappers;
        RZDynamicArray<FileReadJobPayload> payloads;
        wrappers.resize(kFileCount);
        payloads.resize(kFileCount);

        const auto jobStart = Clock::now();
        for (u32 i = 0; i < kFileCount; ++i) {
            payloads[i].jobWrapper     = &wrappers[i];
            payloads[i].path           = files.files[i].path.c_str();
            payloads[i].totalBytes     = &totalBytes;
            payloads[i].completedCount = &completed;
            payloads[i].failures       = &failures;
            PrepareJob(wrappers[i], &payloads[i], FileAsyncReadJob);
            rz_job_system_submit_job(&wrappers[i].job);
        }

        rz_job_system_wait_for_all();
        const auto jobMicros = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - jobStart).count();

        EXPECT_EQ(failures.load(), 0u);
        EXPECT_EQ(completed.load(), kFileCount);
        EXPECT_EQ(totalBytes.load(), files.totalBytes);

        const double speedup = serialMicros / static_cast<double>(jobMicros);

        std::printf("[macOS] Workers: %u | Sequential: %lldus (%zu bytes) | submit: %lldus (%llu bytes) | Speedup: %.2fx\n", RZJobSystemFixture::kWorkerCount, static_cast<long long>(serialMicros), serialBytesRead, static_cast<long long>(jobMicros), static_cast<unsigned long long>(totalBytes.load()), speedup);

        // Submit should not be dramatically slower than serial; allow wide slack for CI and filesystem caching.
        EXPECT_LT(jobMicros, serialMicros * 4 + 100000);
    }

    TEST_F(RZJobSystemHighWorkerFixture, MacSerialGlobalVsSpawnedFileReads)
    {
        constexpr u32   kFileCount     = 150u;
        constexpr size_t kMaxFileSizeB = 256u * 1024u;

        TempFileSet files = CreateTempFilesForIoTest(kFileCount, kMaxFileSizeB);
        TempDirGuard cleanup(files.baseDir);
        ASSERT_EQ(files.files.size(), kFileCount);

        const auto serialStart = Clock::now();
        const size_t serialBytes = SequentialReadBytes(files.files);
        const auto serialMicros  = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - serialStart).count();
        ASSERT_EQ(serialBytes, files.totalBytes);

        std::atomic<uint64_t> totalBytes{0u};
        std::atomic<uint32_t> completed{0u};
        std::atomic<uint32_t> failures{0u};

        RZDynamicArray<JobWrapper>         wrappers;
        RZDynamicArray<FileReadJobPayload> payloads;
        wrappers.resize(kFileCount);
        payloads.resize(kFileCount);

        // Global submit baseline.
        const auto submitStart = Clock::now();
        for (u32 i = 0; i < kFileCount; ++i) {
            payloads[i].jobWrapper     = &wrappers[i];
            payloads[i].path           = files.files[i].path.c_str();
            payloads[i].totalBytes     = &totalBytes;
            payloads[i].completedCount = &completed;
            payloads[i].failures       = &failures;
            PrepareJob(wrappers[i], &payloads[i], FileAsyncReadJob);
            rz_job_system_submit_job(&wrappers[i].job);
        }
        rz_job_system_wait_for_all();
        const auto submitMicros = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - submitStart).count();

        EXPECT_EQ(failures.load(), 0u);
        EXPECT_EQ(completed.load(), kFileCount);
        EXPECT_EQ(totalBytes.load(), files.totalBytes);

        // Reset counters for master-spawned jobs.
        totalBytes.store(0u);
        completed.store(0u);
        failures.store(0u);

        for (u32 i = 0; i < kFileCount; ++i)
            PrepareJob(wrappers[i], &payloads[i], FileAsyncReadJob);

        JobWrapper           masterWrapper{};
        MasterKickoffPayload masterPayload{&masterWrapper, &wrappers, &payloads};
        PrepareJob(masterWrapper, &masterPayload, FileReadAsyncMasterKickoff);

        const auto masterStart = Clock::now();
        rz_job_system_submit_job(&masterWrapper.job);
        rz_job_system_wait_for_all();
        const auto masterMicros = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - masterStart).count();

        EXPECT_EQ(failures.load(), 0u);
        EXPECT_EQ(completed.load(), kFileCount);
        EXPECT_EQ(totalBytes.load(), files.totalBytes);

        const double submitSpeedup = serialMicros / static_cast<double>(submitMicros);
        const double masterSpeedup = serialMicros / static_cast<double>(masterMicros);

        std::printf("[macOS] Workers: %u | Serial: %lldus | submit: %lldus | master-spawn: %lldus | bytes: %llu | Submit speedup: %.2fx | Master speedup: %.2fx\n", RZJobSystemHighWorkerFixture::kWorkerCount, static_cast<long long>(serialMicros), static_cast<long long>(submitMicros), static_cast<long long>(masterMicros), static_cast<unsigned long long>(totalBytes.load()), submitSpeedup, masterSpeedup);

        EXPECT_LT(submitMicros, serialMicros * 4 + 100000);
        EXPECT_LT(masterMicros, serialMicros * 4 + 100000);
    }

    TEST_F(RZJobSystemHighWorkerFixture, MacAggressiveFileReadsHighWorkers)
    {
        constexpr u32   kFileCount     = 256u;
        constexpr size_t kMaxFileSizeB = 512 * 1024u;

        TempFileSet files = CreateTempFilesForIoTest(kFileCount, kMaxFileSizeB);
        TempDirGuard cleanup(files.baseDir);
        ASSERT_EQ(files.files.size(), kFileCount);

        const auto serialStart  = Clock::now();
        const size_t serialRead = SequentialReadBytes(files.files);
        const auto serialMicros = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - serialStart).count();
        ASSERT_EQ(serialRead, files.totalBytes);

        std::atomic<uint64_t> totalBytes{0u};
        std::atomic<uint32_t> completed{0u};
        std::atomic<uint32_t> failures{0u};

        RZDynamicArray<JobWrapper>         wrappers;
        RZDynamicArray<FileReadJobPayload> payloads;
        wrappers.resize(kFileCount);
        payloads.resize(kFileCount);

        const auto submitStart = Clock::now();
        for (u32 i = 0; i < kFileCount; ++i) {
            payloads[i].jobWrapper     = &wrappers[i];
            payloads[i].path           = files.files[i].path.c_str();
            payloads[i].totalBytes     = &totalBytes;
            payloads[i].completedCount = &completed;
            payloads[i].failures       = &failures;
            PrepareJob(wrappers[i], &payloads[i], FileAsyncReadJob);
            rz_job_system_submit_job(&wrappers[i].job);
        }
        rz_job_system_wait_for_all();
        const auto submitMicros = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - submitStart).count();

        EXPECT_EQ(failures.load(), 0u);
        EXPECT_EQ(completed.load(), kFileCount);
        EXPECT_EQ(totalBytes.load(), files.totalBytes);

        const double submitSpeedup = serialMicros / static_cast<double>(submitMicros);

        std::printf("[macOS][Aggressive] Workers: %u | Serial: %lldus | submit: %lldus | bytes: %llu | Submit speedup: %.2fx\n", RZJobSystemHighWorkerFixture::kWorkerCount, static_cast<long long>(serialMicros), static_cast<long long>(submitMicros), static_cast<unsigned long long>(totalBytes.load()), submitSpeedup);

        EXPECT_LT(submitMicros, serialMicros * 4 + 150000);
    }
}    // namespace Razix
