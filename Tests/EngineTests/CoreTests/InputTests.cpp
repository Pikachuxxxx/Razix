// InputTests.cpp
// Unit tests for the RZInput class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZKeyCodes.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZInputTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Note: RZInput requires platform-specific implementation
            // These tests focus on API interface validation
        }

        void TearDown() override
        {
            // Cleanup any resources
        }
    };

    // Test case for input manager selection
    TEST_F(RZInputTests, InputManagerSelection)
    {
        // Test that GLFW input manager can be selected
        // This should not throw or crash
        EXPECT_NO_THROW({
            RZInput::SelectGLFWInputManager();
        });
    }

    // Test case for keyboard input API
    TEST_F(RZInputTests, KeyboardInputAPI)
    {
        // Test that keyboard input methods are accessible
        // Note: Without actual input setup, these will likely return default values
        
        // Test key state checking methods exist and can be called
        EXPECT_NO_THROW({
            bool pressed = RZInput::IsKeyPressed(KeyCode::Key::A);
            bool released = RZInput::IsKeyReleased(KeyCode::Key::A);
            bool held = RZInput::IsKeyHeld(KeyCode::Key::A);
            
            // These are interface tests - actual values depend on input state
            (void)pressed; (void)released; (void)held; // Suppress unused variable warnings
        });
        
        // Test with different key codes
        EXPECT_NO_THROW({
            RZInput::IsKeyPressed(KeyCode::Key::Space);
            RZInput::IsKeyPressed(KeyCode::Key::Enter);
            RZInput::IsKeyPressed(KeyCode::Key::Escape);
        });
    }

    // Test case for mouse input API
    TEST_F(RZInputTests, MouseInputAPI)
    {
        // Test mouse button checking methods
        EXPECT_NO_THROW({
            bool pressed = RZInput::IsMouseButtonPressed(KeyCode::MouseKey::ButtonLeft);
            bool released = RZInput::IsMouseButtonReleased(KeyCode::MouseKey::ButtonLeft);
            bool held = RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonLeft);
            
            (void)pressed; (void)released; (void)held;
        });
        
        // Test mouse position methods
        EXPECT_NO_THROW({
            auto position = RZInput::GetMousePosition();
            f32 x = RZInput::GetMouseX();
            f32 y = RZInput::GetMouseY();
            
            // Position should be a valid pair
            EXPECT_TRUE(position.first >= 0.0f || position.first < 0.0f); // Always true, just checking no crash
            EXPECT_TRUE(position.second >= 0.0f || position.second < 0.0f);
            
            (void)x; (void)y;
        });
        
        // Test different mouse buttons
        EXPECT_NO_THROW({
            RZInput::IsMouseButtonPressed(KeyCode::MouseKey::ButtonLeft);
            RZInput::IsMouseButtonPressed(KeyCode::MouseKey::ButtonRight);
            RZInput::IsMouseButtonPressed(KeyCode::MouseKey::ButtonMiddle);
        });
    }

    // Test case for gamepad input API
    TEST_F(RZInputTests, GamepadInputAPI)
    {
        // Test gamepad connection check
        EXPECT_NO_THROW({
            bool connected = RZInput::IsGamepadConnected();
            (void)connected;
        });
        
        // Test analog stick input methods
        EXPECT_NO_THROW({
            f32 leftH = RZInput::GetJoyLeftStickHorizontal();
            f32 leftV = RZInput::GetJoyLeftStickVertical();
            f32 rightH = RZInput::GetJoyRightStickHorizontal();
            f32 rightV = RZInput::GetJoyRightStickVertical();
            
            // Values should be valid floats (within reasonable range for stick input)
            EXPECT_TRUE(leftH >= -2.0f && leftH <= 2.0f);   // Allow some margin for implementation details
            EXPECT_TRUE(leftV >= -2.0f && leftV <= 2.0f);
            EXPECT_TRUE(rightH >= -2.0f && rightH <= 2.0f);
            EXPECT_TRUE(rightV >= -2.0f && rightV <= 2.0f);
        });
        
        // Test D-pad input methods
        EXPECT_NO_THROW({
            f32 dpadH = RZInput::GetJoyDPadHorizontal();
            f32 dpadV = RZInput::GetJoyDPadVertical();
            
            (void)dpadH; (void)dpadV;
        });
    }

    // Test case for PS5 DualSense button API
    TEST_F(RZInputTests, DualSenseButtonAPI)
    {
        // Test PS5 controller button methods
        EXPECT_NO_THROW({
            bool cross = RZInput::IsCrossPressed();
            bool circle = RZInput::IsCirclePressed();
            bool triangle = RZInput::IsTrianglePressed();
            bool square = RZInput::IsSquarePressed();
            
            (void)cross; (void)circle; (void)triangle; (void)square;
        });
    }

    // Test case for key code enum usage
    TEST_F(RZInputTests, KeyCodeEnumUsage)
    {
        // Test that key code enums can be used properly
        KeyCode::Key testKey = KeyCode::Key::A;
        EXPECT_EQ(testKey, KeyCode::Key::A);
        
        KeyCode::MouseKey testMouse = KeyCode::MouseKey::ButtonLeft;
        EXPECT_EQ(testMouse, KeyCode::MouseKey::ButtonLeft);
        
        // Test that different keys have different values
        EXPECT_NE(KeyCode::Key::A, KeyCode::Key::B);
        EXPECT_NE(KeyCode::MouseKey::ButtonLeft, KeyCode::MouseKey::ButtonRight);
    }

    // Test case for input state consistency
    TEST_F(RZInputTests, InputStateConsistency)
    {
        // Test that input state methods don't crash with various inputs
        std::vector<KeyCode::Key> testKeys = {
            KeyCode::Key::A, KeyCode::Key::Z, KeyCode::Key::Space,
            KeyCode::Key::Enter, KeyCode::Key::Escape, KeyCode::Key::F1
        };
        
        for (auto key : testKeys) {
            EXPECT_NO_THROW({
                RZInput::IsKeyPressed(key);
                RZInput::IsKeyReleased(key);
                RZInput::IsKeyHeld(key);
            });
        }
        
        std::vector<KeyCode::MouseKey> testMouseKeys = {
            KeyCode::MouseKey::ButtonLeft, 
            KeyCode::MouseKey::ButtonRight, 
            KeyCode::MouseKey::ButtonMiddle
        };
        
        for (auto mouseKey : testMouseKeys) {
            EXPECT_NO_THROW({
                RZInput::IsMouseButtonPressed(mouseKey);
                RZInput::IsMouseButtonReleased(mouseKey);
                RZInput::IsMouseButtonHeld(mouseKey);
            });
        }
    }

}    // namespace Razix