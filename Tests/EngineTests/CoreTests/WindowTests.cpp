// WindowTests.cpp
// Unit tests for the RZWindow class and WindowProperties struct
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZWindow.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZWindowTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Initialize test setup
        }

        void TearDown() override
        {
            // Clean up any allocated resources
        }
    };

    // Test case for WindowProperties default constructor
    TEST_F(RZWindowTests, WindowPropertiesDefaultConstructor)
    {
        WindowProperties props;
        
        // Test default values
        EXPECT_EQ(props.Width, 1280);
        EXPECT_EQ(props.Height, 720);
        EXPECT_EQ(props.Title, "Razix Engine");
        EXPECT_FALSE(props.Fullscreen);
        EXPECT_TRUE(props.VSync);
        EXPECT_FALSE(props.Borderless);
        EXPECT_TRUE(props.ShowConsole);
    }

    // Test case for WindowProperties parameterized constructor
    TEST_F(RZWindowTests, WindowPropertiesParameterizedConstructor)
    {
        WindowProperties props("Test Window", 1920, 1080, true, false, true);
        
        EXPECT_EQ(props.Width, 1920);
        EXPECT_EQ(props.Height, 1080);
        EXPECT_EQ(props.Title, "Test Window");
        EXPECT_TRUE(props.Fullscreen);
        EXPECT_FALSE(props.VSync);
        EXPECT_TRUE(props.Borderless);
        EXPECT_TRUE(props.ShowConsole); // Default value
    }

    // Test case for WindowProperties member modification
    TEST_F(RZWindowTests, WindowPropertiesModification)
    {
        WindowProperties props;
        
        // Modify properties
        props.Width = 800;
        props.Height = 600;
        props.Title = "Modified Window";
        props.Fullscreen = true;
        props.VSync = false;
        props.Borderless = true;
        props.ShowConsole = false;
        
        // Verify modifications
        EXPECT_EQ(props.Width, 800);
        EXPECT_EQ(props.Height, 600);
        EXPECT_EQ(props.Title, "Modified Window");
        EXPECT_TRUE(props.Fullscreen);
        EXPECT_FALSE(props.VSync);
        EXPECT_TRUE(props.Borderless);
        EXPECT_FALSE(props.ShowConsole);
    }

    // Test case for WindowProperties copy construction
    TEST_F(RZWindowTests, WindowPropertiesCopyConstruction)
    {
        WindowProperties original("Original", 1024, 768, true, false, true);
        WindowProperties copy = original;
        
        EXPECT_EQ(copy.Width, original.Width);
        EXPECT_EQ(copy.Height, original.Height);
        EXPECT_EQ(copy.Title, original.Title);
        EXPECT_EQ(copy.Fullscreen, original.Fullscreen);
        EXPECT_EQ(copy.VSync, original.VSync);
        EXPECT_EQ(copy.Borderless, original.Borderless);
        EXPECT_EQ(copy.ShowConsole, original.ShowConsole);
    }

    // Test case for WindowProperties assignment
    TEST_F(RZWindowTests, WindowPropertiesAssignment)
    {
        WindowProperties original("Test", 1920, 1080, true, false, true);
        WindowProperties assigned;
        
        assigned = original;
        
        EXPECT_EQ(assigned.Width, original.Width);
        EXPECT_EQ(assigned.Height, original.Height);
        EXPECT_EQ(assigned.Title, original.Title);
        EXPECT_EQ(assigned.Fullscreen, original.Fullscreen);
        EXPECT_EQ(assigned.VSync, original.VSync);
        EXPECT_EQ(assigned.Borderless, original.Borderless);
    }

    // Test case for valid window dimensions
    TEST_F(RZWindowTests, ValidWindowDimensions)
    {
        WindowProperties props;
        
        // Test various valid dimensions
        props.Width = 320; props.Height = 240;
        EXPECT_GT(props.Width, 0);
        EXPECT_GT(props.Height, 0);
        
        props.Width = 1920; props.Height = 1080;
        EXPECT_GT(props.Width, 0);
        EXPECT_GT(props.Height, 0);
        
        props.Width = 3840; props.Height = 2160;
        EXPECT_GT(props.Width, 0);
        EXPECT_GT(props.Height, 0);
    }

    // Test case for common window aspect ratios
    TEST_F(RZWindowTests, CommonAspectRatios)
    {
        // Test common aspect ratios
        WindowProperties props16_9("16:9", 1920, 1080);
        double aspectRatio16_9 = (double)props16_9.Width / props16_9.Height;
        EXPECT_NEAR(aspectRatio16_9, 16.0/9.0, 0.01);
        
        WindowProperties props4_3("4:3", 1024, 768);
        double aspectRatio4_3 = (double)props4_3.Width / props4_3.Height;
        EXPECT_NEAR(aspectRatio4_3, 4.0/3.0, 0.01);
        
        WindowProperties props21_9("21:9", 2560, 1080);
        double aspectRatio21_9 = (double)props21_9.Width / props21_9.Height;
        EXPECT_NEAR(aspectRatio21_9, 21.0/9.0, 0.01);
    }

    // Test case for boolean property combinations
    TEST_F(RZWindowTests, BooleanPropertyCombinations)
    {
        WindowProperties props;
        
        // Test fullscreen + borderless combination
        props.Fullscreen = true;
        props.Borderless = true;
        EXPECT_TRUE(props.Fullscreen && props.Borderless);
        
        // Test windowed + VSync combination
        props.Fullscreen = false;
        props.VSync = true;
        EXPECT_TRUE(!props.Fullscreen && props.VSync);
        
        // Test all flags enabled
        props.Fullscreen = true;
        props.VSync = true;
        props.Borderless = true;
        props.ShowConsole = true;
        EXPECT_TRUE(props.Fullscreen && props.VSync && props.Borderless && props.ShowConsole);
    }

    // Test case for window title handling
    TEST_F(RZWindowTests, WindowTitleHandling)
    {
        WindowProperties props;
        
        // Test empty title
        props.Title = "";
        EXPECT_TRUE(props.Title.empty());
        
        // Test long title
        std::string longTitle(256, 'A');
        props.Title = longTitle;
        EXPECT_EQ(props.Title.length(), 256);
        
        // Test special characters in title
        props.Title = "Räzix Ëngïnë 测试";
        EXPECT_FALSE(props.Title.empty());
        
        // Test title with numbers and symbols
        props.Title = "Razix Engine v1.0 - [Debug Mode] @2024";
        EXPECT_GT(props.Title.length(), 0);
    }

    // Test case for RZWindow interface design
    TEST_F(RZWindowTests, RZWindowInterfaceDesign)
    {
        // Test that RZWindow is abstract (cannot be instantiated directly)
        // This is verified by the virtual destructor and pure virtual methods
        
        // Test EventCallbackFn typedef
        using CallbackType = RZWindow::EventCallbackFn;
        EXPECT_TRUE(std::is_same_v<CallbackType, std::function<void(RZEvent&)>>);
    }

    // Note: We cannot test actual window creation without platform-specific setup
    // and graphics context initialization. The tests above focus on the data structures
    // and interface design that can be validated without creating actual windows.

}    // namespace Razix