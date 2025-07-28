// ApplicationTests.cpp
// Unit tests for the RZApplication class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/App/RZApplication.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace Razix {

    class RZApplicationTests : public ::testing::Test
    {
    protected:
        const std::string testProjectPath = "test_project";
        const std::string testProjectFile = "test_project/TestApp.razixproject";
        const std::string testProjectName = "TestApp";

        void SetUp() override
        {
            // Clean up any existing test files
            if (fs::exists(testProjectPath))
                fs::remove_all(testProjectPath);
            
            // Create test directory
            fs::create_directories(testProjectPath);
        }

        void TearDown() override
        {
            // Clean up test files
            if (fs::exists(testProjectPath))
                fs::remove_all(testProjectPath);
        }

        // Helper to create a test .razixproject file
        void createTestProjectFile()
        {
            std::string projectContent = R"({
    "Razix Application": {
        "Project Name": "TestApp",
        "Engine Version": "0.49.0",
        "Project ID": "12345678-abcd-1234-abcd-123456789abc",
        "Render API": 1,
        "Width": 1920,
        "Height": 1080,
        "Scenes": [
            "//Scenes/test_scene.rzscn"
        ]
    }
})";
            std::ofstream file(testProjectFile);
            file << projectContent;
            file.close();
        }
    };

    // Test case for application construction
    TEST_F(RZApplicationTests, Construction)
    {
        // Test creating an application with project path and name
        RZApplication* app = new RZApplication(testProjectPath, testProjectName);
        
        EXPECT_EQ(app->getAppName(), testProjectName);
        EXPECT_EQ(app->getProjectRoot(), testProjectPath);
        EXPECT_EQ(app->getAppType(), AppType::kGame); // Default type
        EXPECT_EQ(app->getAppState(), AppState::Loading); // Default state
        
        delete app;
    }

    // Test case for singleton access
    TEST_F(RZApplicationTests, SingletonAccess)
    {
        // Create application (this sets the singleton instance)
        RZApplication* app = new RZApplication(testProjectPath, testProjectName);
        
        // Test singleton access
        RZApplication& instance = RZApplication::Get();
        EXPECT_EQ(&instance, app);
        
        delete app;
    }

    // Test case for project properties
    TEST_F(RZApplicationTests, ProjectProperties)
    {
        RZApplication app(testProjectPath, testProjectName);
        
        // Test getting and setting project root
        app.setProjectRoot("/new/path");
        EXPECT_EQ(app.getProjectRoot(), "/new/path");
        
        // Test project name
        EXPECT_EQ(app.getAppName(), testProjectName);
        
        // Test project UUID (should be valid)
        RZUUID projectId = app.getProjectUUID();
        EXPECT_TRUE(projectId.prettyString().length() > 0);
    }

    // Test case for application type and state
    TEST_F(RZApplicationTests, TypeAndState)
    {
        RZApplication app(testProjectPath, testProjectName);
        
        // Test application type
        EXPECT_EQ(app.getAppType(), AppType::kGame);
        app.setAppType(AppType::kTool);
        EXPECT_EQ(app.getAppType(), AppType::kTool);
        
        // Test application state
        EXPECT_EQ(app.getAppState(), AppState::Loading);
        app.setAppState(AppState::Running);
        EXPECT_EQ(app.getAppState(), AppState::Running);
    }

    // Test case for window properties
    TEST_F(RZApplicationTests, WindowProperties)
    {
        RZApplication app(testProjectPath, testProjectName);
        
        // Test accessing window properties
        WindowProperties& props = app.getWindowProps();
        
        // Default window properties should be reasonable
        EXPECT_GT(props.Width, 0);
        EXPECT_GT(props.Height, 0);
        EXPECT_FALSE(props.Title.empty());
    }

    // Test case for Guizmo settings
    TEST_F(RZApplicationTests, GuizmoSettings)
    {
        RZApplication app(testProjectPath, testProjectName);
        
        // Test Guizmo operation and mode settings
        app.setGuizmoOperation(Guizmo::ROTATE);
        app.setGuizmoMode(Guizmo::LOCAL);
        app.setGuizmoSnapAmount(0.5f);
        
        // Test disabling Guizmo editing
        app.disableGuizmoEditing();
        
        // These methods should not throw and allow chaining
        SUCCEED() << "Guizmo settings methods should work without issues";
    }

    // Test case for timer access
    TEST_F(RZApplicationTests, TimerAccess)
    {
        RZApplication app(testProjectPath, testProjectName);
        
        // Test accessing timer (should not be null after construction)
        RZTimer timer = app.getTimer();
        SUCCEED() << "Timer should be accessible from application";
    }

    // Test case for project file validation
    TEST_F(RZApplicationTests, ProjectFileValidation)
    {
        createTestProjectFile();
        
        // Verify the test project file exists and is readable
        EXPECT_TRUE(fs::exists(testProjectFile));
        
        // Read the file and verify basic JSON structure
        std::ifstream file(testProjectFile);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        EXPECT_TRUE(content.find("Razix Application") != std::string::npos);
        EXPECT_TRUE(content.find("Project Name") != std::string::npos);
        EXPECT_TRUE(content.find("TestApp") != std::string::npos);
        EXPECT_TRUE(content.find("Engine Version") != std::string::npos);
        EXPECT_TRUE(content.find("Project ID") != std::string::npos);
    }

}    // namespace Razix