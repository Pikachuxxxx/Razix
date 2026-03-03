// FileWatcherTests.cpp
// Unit tests for RZFileWatcher (directory/file watching + event integration)
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZFileSystem.h"

#include "Razix/Events/RZEvent.h"
#include "Razix/Events/RZFileChangeEvent.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace Razix {

    class RZFileWatcherTests : public ::testing::Test
    {
    protected:
        const std::string watchDir  = "fw_test_dir";
        const std::string watchFile = "fw_test_dir/watched.txt";

        void SetUp() override
        {
            if (fs::exists(watchDir))
                fs::remove_all(watchDir);
            fs::create_directory(watchDir);
        }

        void TearDown() override
        {
            if (fs::exists(watchDir))
                fs::remove_all(watchDir);
        }

        static void sleep_ms(int ms)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
    };

    // -------------------------------------------------------------------------
    // Test 1: Detect a newly created file inside a watched directory
    // -------------------------------------------------------------------------
    TEST_F(RZFileWatcherTests, DirectoryWatcher_DetectsFileAdded)
    {
        RZFileWatcher* watcher = RZFileSystem::CreateFileWatcherForDirectory(watchDir.c_str());
        ASSERT_NE(watcher, nullptr);

        // Create a file inside the watched directory (explicitly closed before polling)
        sleep_ms(50);
        {
            std::ofstream f(watchFile);
            f << "hello";
            f.close();
        }
        sleep_ms(150);

        RZFileChange changes[64];
        int          count = 0;
        // Poll up to 3 times to drain OS queue
        for (int i = 0; i < 3 && count == 0; ++i) {
            watcher->poll(watcher, changes, &count, 64);
            if (count == 0)
                sleep_ms(50);
        }

        EXPECT_GT(count, 0);
        bool foundAdded = false;
        for (int i = 0; i < count; ++i) {
            if (changes[i].type == RZFileChangeType::Added) {
                foundAdded = true;
                break;
            }
        }
        EXPECT_TRUE(foundAdded);

        RZFileSystem::DestroyFileWatcher(watcher);
    }

    // -------------------------------------------------------------------------
    // Test 2: Detect a file modification
    // -------------------------------------------------------------------------
    TEST_F(RZFileWatcherTests, FileWatcher_DetectsModification)
    {
        // Create the file before setting up the watcher so we get a clean state
        {
            std::ofstream f(watchFile);
            f << "initial";
            f.close();
        }
        sleep_ms(50);

        RZFileWatcher* watcher = RZFileSystem::CreateFileWatcherForFile(watchFile.c_str());
        ASSERT_NE(watcher, nullptr);

        // Modify the file (explicitly closed before polling)
        sleep_ms(50);
        {
            std::ofstream f(watchFile, std::ios::trunc);
            f << "modified content";
            f.close();
        }
        sleep_ms(150);

        RZFileChange changes[64];
        int          count = 0;
        for (int i = 0; i < 5 && count == 0; ++i) {
            watcher->poll(watcher, changes, &count, 64);
            if (count == 0)
                sleep_ms(50);
        }

        EXPECT_GT(count, 0);
        bool foundModified = false;
        for (int i = 0; i < count; ++i) {
            if (changes[i].type == RZFileChangeType::Modified) {
                foundModified = true;
                break;
            }
        }
        EXPECT_TRUE(foundModified);

        RZFileSystem::DestroyFileWatcher(watcher);
    }

    // -------------------------------------------------------------------------
    // Test 3: Event integration – file creation triggers RZFileCreatedEvent
    // -------------------------------------------------------------------------

    /**
     * Dispatch file-change events into the RZEventDispatcher based on poll results.
     * This mirrors what the engine's Update() loop would do.
     */
    static void DispatchFileChangeEvents(RZFileWatcher* watcher, RZEventDispatcher& dispatcher)
    {
        RZFileChange changes[64];
        int          count = 0;
        watcher->poll(watcher, changes, &count, 64);
        for (int i = 0; i < count; ++i) {
            switch (changes[i].type) {
                case RZFileChangeType::Modified: {
                    RZFileChangedEvent e(changes[i].path);
                    dispatcher.dispatch(e);
                    break;
                }
                case RZFileChangeType::Added: {
                    RZFileCreatedEvent e(changes[i].path);
                    dispatcher.dispatch(e);
                    break;
                }
                case RZFileChangeType::Removed: {
                    RZFileDeletedEvent e(changes[i].path);
                    dispatcher.dispatch(e);
                    break;
                }
            }
        }
    }

    TEST_F(RZFileWatcherTests, EventIntegration_FileCreationDispatchesEvent)
    {
        RZFileWatcher* watcher = RZFileSystem::CreateFileWatcherForDirectory(watchDir.c_str());
        ASSERT_NE(watcher, nullptr);

        RZEventDispatcher dispatcher;

        bool createdReceived = false;
        dispatcher.registerCallback<RZFileCreatedEvent>([&](RZFileCreatedEvent& e) {
            createdReceived = true;
        });

        // Create the file (explicitly closed before polling)
        sleep_ms(50);
        {
            std::ofstream f(watchFile);
            f << "event test";
            f.close();
        }
        sleep_ms(150);

        // Poll multiple frames until the event arrives or we give up
        for (int frame = 0; frame < 5 && !createdReceived; ++frame) {
            DispatchFileChangeEvents(watcher, dispatcher);
            if (!createdReceived)
                sleep_ms(50);
        }

        EXPECT_TRUE(createdReceived);

        RZFileSystem::DestroyFileWatcher(watcher);
    }

    // -------------------------------------------------------------------------
    // Test 4: Destroy with null / already-null watcher is safe
    // -------------------------------------------------------------------------
    TEST_F(RZFileWatcherTests, DestroyNullWatcher_IsNoOp)
    {
        EXPECT_NO_THROW(RZFileSystem::DestroyFileWatcher(nullptr));
    }

    // -------------------------------------------------------------------------
    // Test 5: Event class accessors return correct path
    // -------------------------------------------------------------------------
    TEST_F(RZFileWatcherTests, FileChangeEvents_PathAccessors)
    {
        const RZString testPath = "some/file.txt";

        RZFileChangedEvent changed(testPath);
        EXPECT_EQ(changed.GetPath(), testPath);
        EXPECT_EQ(changed.GetEventType(), EventType::kFileChanged);

        RZFileCreatedEvent created(testPath);
        EXPECT_EQ(created.GetPath(), testPath);
        EXPECT_EQ(created.GetEventType(), EventType::kFileCreated);

        RZFileDeletedEvent deleted(testPath);
        EXPECT_EQ(deleted.GetPath(), testPath);
        EXPECT_EQ(deleted.GetEventType(), EventType::kFileDeleted);
    }

}    // namespace Razix
