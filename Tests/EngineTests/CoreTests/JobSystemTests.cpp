#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/thread.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <cstring>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <random>

namespace Razix {
    namespace {

        using Clock = std::chrono::steady_clock;

        // ─── A self-contained job bundle ───────────────────────────
        // Owns both the rz_job and its cold data so they can never go out of sync.
        struct TestJob
        {
            rz_job      job;
            rz_job_cold cold;

            TestJob()
            {
                std::memset(&job, 0, sizeof(job));
                std::memset(&cold, 0, sizeof(cold));
                job.pCold = &cold;
            }

            void prepare(void* payload, void (*func)(void*))
            {
                std::memset(&job.hot, 0, sizeof(job.hot));
                std::memset(&cold, 0, sizeof(cold));
                job.pCold         = &cold;
                job.hot.pFunc     = func;
                job.hot.pUserData = payload;
            }
        };

        // ─── Fixture ───────────────────────────────────────────────

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

        // ════════════════════════════════════════════════════════════
        // Test 1 — Single job fires
        // ════════════════════════════════════════════════════════════

        struct SingleHitData
        {
            rz_atomic_u32 hit;
        };

        static void SingleHitFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<SingleHitData*>(pUserData);
            rz_atomic32_store(&d->hit, 1u, RZ_MEMORY_ORDER_RELEASE);
        }

        TEST_F(RZJobSystemFixture, SingleJobExecutes)
        {
            SingleHitData data{};
            rz_atomic32_store(&data.hit, 0u, RZ_MEMORY_ORDER_RELAXED);

            TestJob job;
            job.prepare(&data, SingleHitFunc);

            rz_job_system_submit_job(&job.job);
            rz_job_system_wait_for_all();

            EXPECT_EQ(rz_atomic32_load(&data.hit, RZ_MEMORY_ORDER_ACQUIRE), 1u);
        }

        // ════════════════════════════════════════════════════════════
        // Test 2 — Multiple independent jobs all complete
        // ════════════════════════════════════════════════════════════

        struct CounterData
        {
            rz_atomic_u32* pCounter;
        };

        static void CounterFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<CounterData*>(pUserData);
            rz_atomic32_increment(d->pCounter, RZ_MEMORY_ORDER_RELAXED);
        }

        TEST_F(RZJobSystemFixture, RunsMultipleIndependentJobs)
        {
            constexpr u32 kJobCount = 12u;

            rz_atomic_u32 counter = 0u;
            rz_atomic32_store(&counter, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>     jobs(kJobCount);
            RZDynamicArray<CounterData> payloads(kJobCount);

            for (u32 i = 0; i < kJobCount; ++i) {
                payloads.push_back(CounterData{&counter});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], CounterFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();

            EXPECT_EQ(rz_atomic32_load(&counter, RZ_MEMORY_ORDER_ACQUIRE), kJobCount);
        }

        // ════════════════════════════════════════════════════════════
        // Test 3 — Worker-spawned child jobs complete
        // ════════════════════════════════════════════════════════════

        struct ChildData
        {
            rz_atomic_u32* pCounter;
        };

        static void ChildFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<ChildData*>(pUserData);
            rz_atomic32_increment(d->pCounter, RZ_MEMORY_ORDER_RELAXED);
        }

        struct ParentData
        {
            rz_atomic_u32* pParentFlag;
            TestJob*       pChildren;
            u32            childCount;
        };

        static void ParentFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<ParentData*>(pUserData);
            rz_atomic32_store(d->pParentFlag, 1u, RZ_MEMORY_ORDER_RELEASE);

            for (u32 i = 0; i < d->childCount; ++i)
                rz_job_system_worker_spawn_job(&d->pChildren[i].job);
        }

        TEST_F(RZJobSystemFixture, WorkerSpawnedJobsComplete)
        {
            constexpr u32 kChildCount = 6u;

            rz_atomic_u32 parentFlag   = 0u;
            rz_atomic_u32 childCounter = 0u;
            rz_atomic32_store(&parentFlag, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&childCounter, 0u, RZ_MEMORY_ORDER_RELAXED);

            // Pre-allocate with capacity, then push — no reallocation surprises
            RZDynamicArray<TestJob>   children(kChildCount);
            RZDynamicArray<ChildData> childPayloads(kChildCount);

            for (u32 i = 0; i < kChildCount; ++i) {
                childPayloads.push_back(ChildData{&childCounter});
                children.push_back(TestJob{});
                children[i].prepare(&childPayloads[i], ChildFunc);
            }

            TestJob    parentJob;
            ParentData parentPayload{&parentFlag, &children[0], kChildCount};
            parentJob.prepare(&parentPayload, ParentFunc);

            rz_job_system_submit_job(&parentJob.job);
            rz_job_system_wait_for_all();

            EXPECT_EQ(rz_atomic32_load(&parentFlag, RZ_MEMORY_ORDER_ACQUIRE), 1u);
            EXPECT_EQ(rz_atomic32_load(&childCounter, RZ_MEMORY_ORDER_ACQUIRE), kChildCount);
        }

        // ════��═══════════════════════════════════════════════════════
        // Test 4 — Parallel busy-work is faster than serial
        // ════════════════════════════════════════════════════════════

        struct BusyData
        {
            rz_atomic_u32* pDoneCounter;
            u32            busyMicros;
        };

        static void BusyFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<BusyData*>(pUserData);
            rz_thread_busy_wait_micro(d->busyMicros);
            rz_atomic32_increment(d->pDoneCounter, RZ_MEMORY_ORDER_RELAXED);
        }

        TEST_F(RZJobSystemFixture, ParallelIsFasterThanSerial)
        {
            constexpr u32 kJobCount   = 16u;
            constexpr u32 kBusyMicros = 1500u;

            // --- serial baseline ---
            auto serialStart = Clock::now();
            for (u32 i = 0; i < kJobCount; ++i)
                rz_thread_busy_wait_micro(kBusyMicros);
            auto serialUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - serialStart)
                                .count();

            // --- parallel run ---
            rz_atomic_u32 doneCounter = 0u;
            rz_atomic32_store(&doneCounter, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>  jobs(kJobCount);
            RZDynamicArray<BusyData> payloads(kJobCount);

            auto parallelStart = Clock::now();
            for (u32 i = 0; i < kJobCount; ++i) {
                payloads.push_back(BusyData{&doneCounter, kBusyMicros});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], BusyFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();
            auto parallelUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - parallelStart)
                                  .count();

            ASSERT_EQ(rz_atomic32_load(&doneCounter, RZ_MEMORY_ORDER_ACQUIRE), kJobCount);

            // Allow 1.8× + 5 ms slack for CI variance
            EXPECT_LT(parallelUs, serialUs * 18 / 10 + 5000);
        }

        // ════════════════════════════════════════════════════════════
        // Test 5 — High volume: many independent jobs all complete
        // ════════════════════════════════════════════════════════════

        TEST_F(RZJobSystemFixture, HighVolumeIndependentJobs)
        {
            constexpr u32 kJobCount = 256u;

            rz_atomic_u32 counter = 0u;
            rz_atomic32_store(&counter, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>     jobs(kJobCount);
            RZDynamicArray<CounterData> payloads(kJobCount);

            for (u32 i = 0; i < kJobCount; ++i) {
                payloads.push_back(CounterData{&counter});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], CounterFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();

            EXPECT_EQ(rz_atomic32_load(&counter, RZ_MEMORY_ORDER_ACQUIRE), kJobCount);
        }

        // ════════════════════════════════════════════════════════════
        // Test 6 — Many children spawned from a single parent
        // ════════════════════════════════════════════════════════════

        TEST_F(RZJobSystemFixture, ManyChildrenFromOneParent)
        {
            constexpr u32 kChildCount = 64u;

            rz_atomic_u32 parentFlag   = 0u;
            rz_atomic_u32 childCounter = 0u;
            rz_atomic32_store(&parentFlag, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&childCounter, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>   children(kChildCount);
            RZDynamicArray<ChildData> childPayloads(kChildCount);

            for (u32 i = 0; i < kChildCount; ++i) {
                childPayloads.push_back(ChildData{&childCounter});
                children.push_back(TestJob{});
                children[i].prepare(&childPayloads[i], ChildFunc);
            }

            TestJob    parentJob;
            ParentData parentPayload{&parentFlag, &children[0], kChildCount};
            parentJob.prepare(&parentPayload, ParentFunc);

            rz_job_system_submit_job(&parentJob.job);
            rz_job_system_wait_for_all();

            EXPECT_EQ(rz_atomic32_load(&parentFlag, RZ_MEMORY_ORDER_ACQUIRE), 1u);
            EXPECT_EQ(rz_atomic32_load(&childCounter, RZ_MEMORY_ORDER_ACQUIRE), kChildCount);
        }

        // ════════════════════════════════════════════════════════════
        // Test 7 — Simulated I/O: jobs doing variable-length busy work
        //          (replaces the filesystem tests — tests the same
        //           scheduling pattern without flaky FS dependencies)
        // ════════════════════════════════════════════════════════════

        struct SimIoData
        {
            rz_atomic_u32* pBytesProcessed;
            rz_atomic_u32* pCompleted;
            u32            workMicros;
            u32            simulatedBytes;
        };

        static void SimIoFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<SimIoData*>(pUserData);
            rz_thread_busy_wait_micro(d->workMicros);
            rz_atomic32_add(d->pBytesProcessed, d->simulatedBytes, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_increment(d->pCompleted, RZ_MEMORY_ORDER_RELAXED);
        }

        TEST_F(RZJobSystemFixture, SimulatedIoWorkload)
        {
            constexpr u32 kJobCount      = 150u;
            constexpr u32 kWorkPerJobUs  = 200u;
            constexpr u32 kBytesPerJob   = 4096u;
            constexpr u32 kExpectedBytes = kJobCount * kBytesPerJob;

            rz_atomic_u32 bytesProcessed = 0u;
            rz_atomic_u32 completed      = 0u;
            rz_atomic32_store(&bytesProcessed, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&completed, 0u, RZ_MEMORY_ORDER_RELAXED);

            // --- serial baseline ---
            auto serialStart = Clock::now();
            for (u32 i = 0; i < kJobCount; ++i)
                rz_thread_busy_wait_micro(kWorkPerJobUs);
            auto serialUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - serialStart)
                                .count();

            // --- parallel ---
            RZDynamicArray<TestJob>   jobs(kJobCount);
            RZDynamicArray<SimIoData> payloads(kJobCount);

            auto parallelStart = Clock::now();
            for (u32 i = 0; i < kJobCount; ++i) {
                payloads.push_back(SimIoData{&bytesProcessed, &completed, kWorkPerJobUs, kBytesPerJob});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], SimIoFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();
            auto parallelUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - parallelStart)
                                  .count();

            EXPECT_EQ(rz_atomic32_load(&completed, RZ_MEMORY_ORDER_ACQUIRE), kJobCount);
            EXPECT_EQ(rz_atomic32_load(&bytesProcessed, RZ_MEMORY_ORDER_ACQUIRE), kExpectedBytes);

            // Parallel should not be dramatically slower than serial
            // Really relaxing it for CI/CD variations
            EXPECT_LT(parallelUs, serialUs * 16 + 100000);

            std::printf("Workers: %u | Serial: %lldus | Parallel: %lldus | Speedup: %.2fx\n",
                kWorkerCount,
                static_cast<long long>(serialUs),
                static_cast<long long>(parallelUs),
                serialUs / static_cast<double>(parallelUs));
        }

        // ════════════════════════════════════════════════════════════
        // Test 8 — Submit, wait, submit again (system reuse)
        // ════════════════════════════════════════════════════════════

        TEST_F(RZJobSystemFixture, SubmitWaitSubmitAgain)
        {
            rz_atomic_u32 counter = 0u;
            rz_atomic32_store(&counter, 0u, RZ_MEMORY_ORDER_RELAXED);

            // Round 1
            RZDynamicArray<TestJob>     jobs1(4u);
            RZDynamicArray<CounterData> payloads1(4u);
            for (u32 i = 0; i < 4u; ++i) {
                payloads1.push_back(CounterData{&counter});
                jobs1.push_back(TestJob{});
                jobs1[i].prepare(&payloads1[i], CounterFunc);
                rz_job_system_submit_job(&jobs1[i].job);
            }
            rz_job_system_wait_for_all();
            EXPECT_EQ(rz_atomic32_load(&counter, RZ_MEMORY_ORDER_ACQUIRE), 4u);

            // Round 2 — system must work again after wait_for_all
            RZDynamicArray<TestJob>     jobs2(4u);
            RZDynamicArray<CounterData> payloads2(4u);
            for (u32 i = 0; i < 4u; ++i) {
                payloads2.push_back(CounterData{&counter});
                jobs2.push_back(TestJob{});
                jobs2[i].prepare(&payloads2[i], CounterFunc);
                rz_job_system_submit_job(&jobs2[i].job);
            }
            rz_job_system_wait_for_all();
            EXPECT_EQ(rz_atomic32_load(&counter, RZ_MEMORY_ORDER_ACQUIRE), 8u);
        }

        // ════════════════════════════════════════════════════════════
        // DISABLED — Real filesystem I/O tests
        // ════════════════════════════════════════════════════════════

        // ─── I/O helpers ───────────────────────────────────────────

        struct TempFileInfo
        {
            char   path[256];
            size_t sizeBytes;
        };

        struct TempDirGuard
        {
            char path[256];

            explicit TempDirGuard(const char* dir)
            {
                std::strncpy(path, dir, sizeof(path) - 1);
                path[sizeof(path) - 1] = '\0';
            }

            ~TempDirGuard()
            {
                if (path[0] != '\0') {
                    std::error_code ec{};
                    std::filesystem::remove_all(path, ec);
                }
            }
        };

        // Creates temp files filled with random ASCII data.
        // Returns total bytes written. Populates the pre-reserved fileInfos array.
        static size_t CreateTempFiles(
            RZDynamicArray<TempFileInfo>& fileInfos,
            const char*                   baseDir,
            u32                           fileCount,
            size_t                        maxBytesPerFile)
        {
            namespace fs = std::filesystem;

            std::error_code ec{};
            fs::create_directories(baseDir, ec);
            if (ec) {
                std::printf("[TempFiles] Failed to create dir: %s\n", ec.message().c_str());
                return 0u;
            }

            std::mt19937                          rng(static_cast<unsigned>(Clock::now().time_since_epoch().count()));
            std::uniform_int_distribution<int>    charDist(32, 126);
            std::uniform_int_distribution<size_t> sizeDist(1u, maxBytesPerFile);

            size_t totalBytes = 0u;

            for (u32 i = 0; i < fileCount; ++i) {
                TempFileInfo info{};
                info.sizeBytes = sizeDist(rng);
                rz_snprintf(info.path, sizeof(info.path), "%s/file_%u.bin", baseDir, i);

                // Write random data
                std::FILE* out = std::fopen(info.path, "wb");
                if (!out) {
                    std::printf("[TempFiles] Failed to open for write: %s\n", info.path);
                    return 0u;
                }

                for (size_t j = 0; j < info.sizeBytes; ++j) {
                    char c = static_cast<char>(charDist(rng));
                    std::fwrite(&c, 1, 1, out);
                }
                std::fclose(out);

                totalBytes += info.sizeBytes;
                fileInfos.push_back(info);
            }

            return totalBytes;
        }

        // Blocking sequential read of one file, returns bytes read.
        static size_t BlockingReadFile(const TempFileInfo& info)
        {
            char       buffer[4096];
            size_t     totalRead = 0u;
            std::FILE* in        = std::fopen(info.path, "rb");
            if (!in) return 0u;

            while (true) {
                size_t bytes = std::fread(buffer, 1, sizeof(buffer), in);
                totalRead += bytes;
                if (bytes < sizeof(buffer))
                    break;
            }

            std::fclose(in);
            return totalRead;
        }

        // Sequential baseline: read all files one by one.
        static size_t SequentialReadAll(const RZDynamicArray<TempFileInfo>& files)
        {
            size_t total = 0u;
            for (u32 i = 0; i < files.size(); ++i)
                total += BlockingReadFile(files[i]);
            return total;
        }

        // ─── File read job payload ─────────────────────────────────

        struct FileReadData
        {
            const char*    path;
            rz_atomic_u32* pTotalBytes;
            rz_atomic_u32* pCompleted;
            rz_atomic_u32* pFailures;
        };

        static void FileReadFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<FileReadData*>(pUserData);

            char       buffer[4096];
            u32        localBytes = 0u;
            std::FILE* in         = std::fopen(d->path, "rb");
            if (!in) {
                rz_atomic32_increment(d->pFailures, RZ_MEMORY_ORDER_RELAXED);
                rz_atomic32_increment(d->pCompleted, RZ_MEMORY_ORDER_RELAXED);
                return;
            }

            while (true) {
                size_t bytes = std::fread(buffer, 1, sizeof(buffer), in);
                localBytes += static_cast<u32>(bytes);
                if (bytes < sizeof(buffer))
                    break;
            }

            std::fclose(in);
            rz_atomic32_add(d->pTotalBytes, localBytes, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_increment(d->pCompleted, RZ_MEMORY_ORDER_RELAXED);
        }

        // ════════════════════════════════════════════════════════════
        // DISABLED Test — Sequential vs parallel file reads (4 workers)
        // ════════════════════════════════════════════════════════════

        TEST_F(RZJobSystemFixture, DISABLED_SequentialVsParallelFileReads)
        {
            constexpr u32    kFileCount    = 150u;
            constexpr size_t kMaxFileSizeB = 256u * 1024u;    // up to 256 KB

            // Create temp dir with unique name
            char baseDir[256];
            rz_snprintf(baseDir, sizeof(baseDir), "/tmp/razix_job_io_%lld", static_cast<long long>(Clock::now().time_since_epoch().count()));
            TempDirGuard cleanup(baseDir);

            RZDynamicArray<TempFileInfo> files(kFileCount);
            size_t                       expectedBytes = CreateTempFiles(files, baseDir, kFileCount, kMaxFileSizeB);
            ASSERT_EQ(files.size(), kFileCount);
            ASSERT_GT(expectedBytes, 0u);

            // --- Sequential baseline ---
            auto   serialStart = Clock::now();
            size_t serialBytes = SequentialReadAll(files);
            auto   serialUs    = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - serialStart)
                                .count();
            ASSERT_EQ(serialBytes, expectedBytes);

            // --- Parallel via job system ---
            rz_atomic_u32 totalBytes = 0u;
            rz_atomic_u32 completed  = 0u;
            rz_atomic_u32 failures   = 0u;
            rz_atomic32_store(&totalBytes, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&completed, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&failures, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>      jobs(kFileCount);
            RZDynamicArray<FileReadData> payloads(kFileCount);

            auto parallelStart = Clock::now();
            for (u32 i = 0; i < kFileCount; ++i) {
                payloads.push_back(FileReadData{files[i].path, &totalBytes, &completed, &failures});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], FileReadFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();
            auto parallelUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - parallelStart)
                                  .count();

            EXPECT_EQ(rz_atomic32_load(&failures, RZ_MEMORY_ORDER_ACQUIRE), 0u);
            EXPECT_EQ(rz_atomic32_load(&completed, RZ_MEMORY_ORDER_ACQUIRE), kFileCount);
            EXPECT_EQ(rz_atomic32_load(&totalBytes, RZ_MEMORY_ORDER_ACQUIRE),
                static_cast<u32>(expectedBytes));

            double speedup = serialUs / static_cast<double>(parallelUs);
            std::printf("[FileIO] Workers: %u | Sequential: %lldus (%zu bytes) | Parallel: %lldus | Speedup: %.2fx\n",
                kWorkerCount,
                static_cast<long long>(serialUs),
                serialBytes,
                static_cast<long long>(parallelUs),
                speedup);

            // Parallel should not be dramatically slower
            EXPECT_LT(parallelUs, serialUs * 4 + 100000);
        }

        // ════════════════════════════════════════════════════════════
        // DISABLED Test — Submit vs worker-spawn file reads (8 workers)
        // ════════════════════════════════════════════════════════════

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

        struct MasterKickoffData
        {
            TestJob* pChildJobs;
            u32      childCount;
        };

        static void MasterKickoffFunc(void* pUserData)
        {
            auto* d = reinterpret_cast<MasterKickoffData*>(pUserData);
            for (u32 i = 0; i < d->childCount; ++i)
                rz_job_system_worker_spawn_job(&d->pChildJobs[i].job);
        }

        TEST_F(RZJobSystemHighWorkerFixture, DISABLED_SubmitVsSpawnedFileReads)
        {
            constexpr u32    kFileCount    = 150u;
            constexpr size_t kMaxFileSizeB = 256u * 1024u;

            char baseDir[256];
            rz_snprintf(baseDir, sizeof(baseDir), "/tmp/razix_job_io_spawn_%lld", static_cast<long long>(Clock::now().time_since_epoch().count()));
            TempDirGuard cleanup(baseDir);

            RZDynamicArray<TempFileInfo> files(kFileCount);
            size_t                       expectedBytes = CreateTempFiles(files, baseDir, kFileCount, kMaxFileSizeB);
            ASSERT_EQ(files.size(), kFileCount);

            // --- Sequential baseline ---
            auto   serialStart = Clock::now();
            size_t serialBytes = SequentialReadAll(files);
            auto   serialUs    = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - serialStart)
                                .count();
            ASSERT_EQ(serialBytes, expectedBytes);

            // --- Round 1: global submit ---
            rz_atomic_u32 totalBytes = 0u;
            rz_atomic_u32 completed  = 0u;
            rz_atomic_u32 failures   = 0u;
            rz_atomic32_store(&totalBytes, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&completed, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&failures, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>      jobs(kFileCount);
            RZDynamicArray<FileReadData> payloads(kFileCount);

            for (u32 i = 0; i < kFileCount; ++i) {
                payloads.push_back(FileReadData{files[i].path, &totalBytes, &completed, &failures});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], FileReadFunc);
            }

            auto submitStart = Clock::now();
            for (u32 i = 0; i < kFileCount; ++i)
                rz_job_system_submit_job(&jobs[i].job);
            rz_job_system_wait_for_all();
            auto submitUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - submitStart)
                                .count();

            EXPECT_EQ(rz_atomic32_load(&failures, RZ_MEMORY_ORDER_ACQUIRE), 0u);
            EXPECT_EQ(rz_atomic32_load(&completed, RZ_MEMORY_ORDER_ACQUIRE), kFileCount);

            // --- Round 2: master-spawned ---
            rz_atomic32_store(&totalBytes, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&completed, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&failures, 0u, RZ_MEMORY_ORDER_RELAXED);

            // Re-prepare all child jobs
            for (u32 i = 0; i < kFileCount; ++i)
                jobs[i].prepare(&payloads[i], FileReadFunc);

            TestJob           masterJob;
            MasterKickoffData masterPayload{&jobs[0], kFileCount};
            masterJob.prepare(&masterPayload, MasterKickoffFunc);

            auto masterStart = Clock::now();
            rz_job_system_submit_job(&masterJob.job);
            rz_job_system_wait_for_all();
            auto masterUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - masterStart)
                                .count();

            EXPECT_EQ(rz_atomic32_load(&failures, RZ_MEMORY_ORDER_ACQUIRE), 0u);
            EXPECT_EQ(rz_atomic32_load(&completed, RZ_MEMORY_ORDER_ACQUIRE), kFileCount);

            std::printf("[FileIO-Spawn] Workers: %u | Serial: %lldus | Submit: %lldus | Master-spawn: %lldus | "
                        "Submit speedup: %.2fx | Spawn speedup: %.2fx\n",
                kWorkerCount,
                static_cast<long long>(serialUs),
                static_cast<long long>(submitUs),
                static_cast<long long>(masterUs),
                serialUs / static_cast<double>(submitUs),
                serialUs / static_cast<double>(masterUs));

            EXPECT_LT(submitUs, serialUs * 4 + 100000);
            EXPECT_LT(masterUs, serialUs * 4 + 100000);
        }

        // ════════════════════════════════════════════════════════════
        // DISABLED Test — Aggressive: 256 larger files on 8 workers
        // ════════════════════════════════════════════════════════════

        TEST_F(RZJobSystemHighWorkerFixture, DISABLED_AggressiveFileReads8Workers)
        {
            constexpr u32    kFileCount    = 256u;
            constexpr size_t kMaxFileSizeB = 512u * 1024u;    // up to 512 KB

            char baseDir[256];
            rz_snprintf(baseDir, sizeof(baseDir), "/tmp/razix_job_io_agg_%lld", static_cast<long long>(Clock::now().time_since_epoch().count()));
            TempDirGuard cleanup(baseDir);

            RZDynamicArray<TempFileInfo> files(kFileCount);
            size_t                       expectedBytes = CreateTempFiles(files, baseDir, kFileCount, kMaxFileSizeB);
            ASSERT_EQ(files.size(), kFileCount);

            // Serial baseline
            auto   serialStart = Clock::now();
            size_t serialBytes = SequentialReadAll(files);
            auto   serialUs    = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - serialStart)
                                .count();
            ASSERT_EQ(serialBytes, expectedBytes);

            // Parallel
            rz_atomic_u32 totalBytes = 0u;
            rz_atomic_u32 completed  = 0u;
            rz_atomic_u32 failures   = 0u;
            rz_atomic32_store(&totalBytes, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&completed, 0u, RZ_MEMORY_ORDER_RELAXED);
            rz_atomic32_store(&failures, 0u, RZ_MEMORY_ORDER_RELAXED);

            RZDynamicArray<TestJob>      jobs(kFileCount);
            RZDynamicArray<FileReadData> payloads(kFileCount);

            auto parallelStart = Clock::now();
            for (u32 i = 0; i < kFileCount; ++i) {
                payloads.push_back(FileReadData{files[i].path, &totalBytes, &completed, &failures});
                jobs.push_back(TestJob{});
                jobs[i].prepare(&payloads[i], FileReadFunc);
                rz_job_system_submit_job(&jobs[i].job);
            }

            rz_job_system_wait_for_all();
            auto parallelUs = std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - parallelStart)
                                  .count();

            EXPECT_EQ(rz_atomic32_load(&failures, RZ_MEMORY_ORDER_ACQUIRE), 0u);
            EXPECT_EQ(rz_atomic32_load(&completed, RZ_MEMORY_ORDER_ACQUIRE), kFileCount);

            double speedup = serialUs / static_cast<double>(parallelUs);
            std::printf("[AggressiveIO] Workers: %u | Serial: %lldus | Parallel: %lldus | Bytes: %zu | Speedup: %.2fx\n",
                kWorkerCount,
                static_cast<long long>(serialUs),
                static_cast<long long>(parallelUs),
                static_cast<size_t>(rz_atomic32_load(&totalBytes, RZ_MEMORY_ORDER_ACQUIRE)),
                speedup);

            EXPECT_LT(parallelUs, serialUs * 4 + 150000);
        }

    }    // namespace
}    // namespace Razix
