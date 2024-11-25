// VFSTests.cpp
// AI-generated unit tests for the RZVirtualFileSystem classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace Razix {
    namespace Filesystem {
        // Fixture for RZVirtualFileSystem Tests
        class VFSTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                Razix::Debug::RZLog::StartUp();
                // Initialize the virtual file system before running tests
                RZVirtualFileSystem::Get().StartUp();
            }

            void TearDown() override
            {
                // Clean up after each test
                RZVirtualFileSystem::Get().ShutDown();
                Razix::Debug::RZLog::Shutdown();
            }

            // Helper function to create a test file
            void createTestFile(const std::string& virtualPath, const std::string& content)
            {
                // Create a physical file in the corresponding mounted directory
                std::string physicalPath;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, physicalPath);
                fs::create_directories(fs::path(physicalPath).parent_path());

                std::ofstream outFile(physicalPath);
                outFile << content;
                outFile.close();
            }

            // Helper function to check if file exists at physical path
            bool checkFileExists(const std::string& virtualPath)
            {
                std::string physicalPath;
                return RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, physicalPath);
            }
        };

        // Test case for mounting paths and verifying the mount
        // These can change not reliable, think of a better test
//        TEST_F(VFSTests, TestMountingPaths)
//        {
//            // Check if the paths are correctly mounted by verifying the physical path
//            std::string physicalPath;
//            bool        result = RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixRoot", physicalPath, true);
//            EXPECT_TRUE(result);
//            EXPECT_FALSE(physicalPath.empty());
//
//            result = RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent", physicalPath, true);
//            EXPECT_TRUE(result);
//            EXPECT_FALSE(physicalPath.empty());
//        }

        // Test case for mounting new paths and checking resolution
        TEST_F(VFSTests, TestMountNewPaths)
        {
            std::string newVirtualPath  = "RazixNewContent";
            std::string newPhysicalPath = RAZIX_ENGINE_ROOT_DIR + std::string("/Engine/content/");

            // Mount a new path
            RZVirtualFileSystem::Get().mount(newVirtualPath, newPhysicalPath);

            // Check if the new path resolves correctly
            std::string resolvedPath;
            bool        result = RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixNewContent", resolvedPath, true);
            EXPECT_TRUE(result);
            EXPECT_EQ(resolvedPath, newPhysicalPath);
        }

        //// Test case to ensure file write works properly for virtual paths
        //TEST_F(VFSTests, TestWriteFile)
        //{
        //    std::string virtualPath = "//RazixTextures/test_texture.txt";
        //    std::string content     = "Test texture data";

        //    // Write to file
        //    bool writeResult = RZVirtualFileSystem::Get().writeTextFile(virtualPath, content);
        //    EXPECT_TRUE(writeResult);

        //    // Verify that the file exists at the correct location
        //    bool fileExists = checkFileExists(virtualPath);
        //    EXPECT_TRUE(fileExists);
        //}

        //// Test case to check reading a file from a mounted path
        //TEST_F(VFSTests, TestReadTextFile)
        //{
        //    std::string virtualPath = "//RazixShaders/test_shader.txt";
        //    std::string content     = "Test shader code";

        //    // Create the file
        //    createTestFile(virtualPath, content);

        //    // Read the file
        //    std::string fileContent = RZVirtualFileSystem::Get().readTextFile(virtualPath);
        //    EXPECT_EQ(fileContent, content);
        //}
    }    // namespace Filesystem
}    // namespace Razix
