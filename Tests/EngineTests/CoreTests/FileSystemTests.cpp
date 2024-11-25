// FileSystemTests.cpp
// AI-generated unit tests for the RZFileSystem classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZFileSystem.h"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace Razix {

    class RZFileSystemTests : public ::testing::Test
    {
    protected:
        const std::string testDir     = "test_dir";
        const std::string testFile    = "test_dir/test_file.txt";
        const std::string testContent = "This is a test file content.";

        void SetUp() override
        {
            // Ensure clean state before tests
            if (fs::exists(testDir))
                fs::remove_all(testDir);
        }

        void TearDown() override
        {
            // Clean up after tests
            if (fs::exists(testDir))
                fs::remove_all(testDir);
        }
    };

    // Test case for CreateDir
    TEST_F(RZFileSystemTests, TestCreateDir)
    {
        EXPECT_FALSE(RZFileSystem::FolderExists(testDir));
        EXPECT_TRUE(RZFileSystem::CreateDir(testDir));
        EXPECT_TRUE(RZFileSystem::FolderExists(testDir));
    }

    // Test case for FileExists and FolderExists
    TEST_F(RZFileSystemTests, TestFileAndFolderExists)
    {
        // Create directory
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));

        // File should not exist
        EXPECT_FALSE(RZFileSystem::FileExists(testFile));

        // Create a test file
        std::ofstream outFile(testFile);
        outFile << testContent;
        outFile.close();

        // File should exist now
        EXPECT_TRUE(RZFileSystem::FileExists(testFile));
    }

    // Test case for GetFileSize
    TEST_F(RZFileSystemTests, TestGetFileSize)
    {
        // Create directory and file
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));
        std::ofstream outFile(testFile);
        outFile << testContent;
        outFile.close();

        // Get the file size
        EXPECT_EQ(RZFileSystem::GetFileSize(testFile), testContent.size());
    }

    // Test case for ReadFile (buffer)
    TEST_F(RZFileSystemTests, TestReadFileToBuffer)
    {
        // Create directory and file
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));
        std::ofstream outFile(testFile);
        outFile << testContent;
        outFile.close();

        // Read file into buffer
        int64_t         fileSize = RZFileSystem::GetFileSize(testFile);
        std::vector<u8> buffer(fileSize);
        EXPECT_TRUE(RZFileSystem::ReadFile(testFile, buffer.data(), fileSize));

        // Verify file content
        std::string readContent(buffer.begin(), buffer.end());
        EXPECT_EQ(readContent, testContent);
    }

    // Test case for ReadTextFile
    TEST_F(RZFileSystemTests, TestReadTextFile)
    {
        // Create directory and file
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));
        std::ofstream outFile(testFile);
        outFile << testContent;
        outFile.close();

        // Read file as text
        std::string readContent = RZFileSystem::ReadTextFile(testFile);
        EXPECT_EQ(readContent, testContent);
    }

    // Test case for WriteFile (binary buffer)
    TEST_F(RZFileSystemTests, TestWriteFile)
    {
        // Create directory
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));

        // Write binary buffer to file
        std::vector<u8> buffer(testContent.begin(), testContent.end());
        EXPECT_TRUE(RZFileSystem::WriteFile(testFile, buffer.data(), buffer.size()));

        // Verify file content
        std::ifstream inFile(testFile);
        std::string   fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        EXPECT_EQ(fileContent, testContent);
    }

    // Test case for WriteTextFile
    TEST_F(RZFileSystemTests, TestWriteTextFile)
    {
        // Create directory
        ASSERT_TRUE(RZFileSystem::CreateDir(testDir));

        // Write text content to file
        EXPECT_TRUE(RZFileSystem::WriteTextFile(testFile, testContent));

        // Verify file content
        std::ifstream inFile(testFile);
        std::string   fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        EXPECT_EQ(fileContent, testContent);
    }

    // Test case for IsRelativePath and IsAbsolutePath
    TEST_F(RZFileSystemTests, TestPathTypes)
    {
        // Relative path
        std::string relativePath = "relative/test/path.txt";
        EXPECT_TRUE(RZFileSystem::IsRelativePath(relativePath.c_str()));
        EXPECT_FALSE(RZFileSystem::IsAbsolutePath(relativePath.c_str()));

        // Absolute path
        std::string absolutePath = fs::absolute("absolute/test/path.txt").string();
        EXPECT_FALSE(RZFileSystem::IsRelativePath(absolutePath.c_str()));
        EXPECT_TRUE(RZFileSystem::IsAbsolutePath(absolutePath.c_str()));
    }
}    // namespace Razix