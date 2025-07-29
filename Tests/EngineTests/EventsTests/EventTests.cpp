// EventTests.cpp
// Unit tests for the Razix Event System
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Events/RZEvent.h"
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZEventTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup for event tests
        }

        void TearDown() override
        {
            // Cleanup for event tests
        }
    };

    // Test case for EventType enum values
    TEST_F(RZEventTests, EventTypeEnumValues)
    {
        // Test that all expected event types exist
        EXPECT_EQ(static_cast<int>(EventType::kNone), 0);
        
        // Window events
        EXPECT_TRUE(static_cast<int>(EventType::kWindowClose) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kWindowResize) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kWindowFocus) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kWindowLostFocus) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kWindowMoved) > 0);
        
        // App events
        EXPECT_TRUE(static_cast<int>(EventType::kAppTick) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kAppUpdate) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kAppRender) > 0);
        
        // Input events
        EXPECT_TRUE(static_cast<int>(EventType::kKeyPressed) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kKeyReleased) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kMouseButtonPressed) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kMouseButtonReleased) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kMouseMoved) > 0);
        EXPECT_TRUE(static_cast<int>(EventType::kMouseScrolled) > 0);
    }

    // Test case for WindowCloseEvent
    TEST_F(RZEventTests, WindowCloseEvent)
    {
        WindowCloseEvent event;
        
        EXPECT_EQ(event.GetEventType(), EventType::kWindowClose);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kApplication));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kWindow));
        EXPECT_FALSE(event.IsHandled());
        
        // Test name
        std::string name = event.GetName();
        EXPECT_FALSE(name.empty());
        EXPECT_TRUE(name.find("WindowClose") != std::string::npos);
    }

    // Test case for WindowResizeEvent
    TEST_F(RZEventTests, WindowResizeEvent)
    {
        u32 width = 1920, height = 1080;
        WindowResizeEvent event(width, height);
        
        EXPECT_EQ(event.GetEventType(), EventType::kWindowResize);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kApplication));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kWindow));
        
        EXPECT_EQ(event.GetWidth(), width);
        EXPECT_EQ(event.GetHeight(), height);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("WindowResize") != std::string::npos);
        EXPECT_TRUE(str.find(std::to_string(width)) != std::string::npos);
        EXPECT_TRUE(str.find(std::to_string(height)) != std::string::npos);
    }

    // Test case for KeyPressedEvent
    TEST_F(RZEventTests, KeyPressedEvent)
    {
        Keys keycode = Keys::KEY_A;
        int repeatCount = 1;
        KeyPressedEvent event(keycode, repeatCount);
        
        EXPECT_EQ(event.GetEventType(), EventType::kKeyPressed);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kKeyboard));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kInput));
        
        EXPECT_EQ(event.GetKeyCode(), keycode);
        EXPECT_EQ(event.GetRepeatCount(), repeatCount);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("KeyPressed") != std::string::npos);
        EXPECT_TRUE(str.find(std::to_string(static_cast<int>(keycode))) != std::string::npos);
    }

    // Test case for KeyReleasedEvent
    TEST_F(RZEventTests, KeyReleasedEvent)
    {
        Keys keycode = Keys::KEY_SPACE;
        KeyReleasedEvent event(keycode);
        
        EXPECT_EQ(event.GetEventType(), EventType::kKeyReleased);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kKeyboard));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kInput));
        
        EXPECT_EQ(event.GetKeyCode(), keycode);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("KeyReleased") != std::string::npos);
    }

    // Test case for MouseButtonPressedEvent
    TEST_F(RZEventTests, MouseButtonPressedEvent)
    {
        MouseCode button = MouseCode::MOUSE_BUTTON_LEFT;
        MouseButtonPressedEvent event(button);
        
        EXPECT_EQ(event.GetEventType(), EventType::kMouseButtonPressed);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kMouse));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kInput));
        
        EXPECT_EQ(event.GetMouseButton(), button);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("MouseButtonPressed") != std::string::npos);
    }

    // Test case for MouseMovedEvent
    TEST_F(RZEventTests, MouseMovedEvent)
    {
        f32 x = 100.5f, y = 200.7f;
        MouseMovedEvent event(x, y);
        
        EXPECT_EQ(event.GetEventType(), EventType::kMouseMoved);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kMouse));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kInput));
        
        EXPECT_FLOAT_EQ(event.GetX(), x);
        EXPECT_FLOAT_EQ(event.GetY(), y);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("MouseMoved") != std::string::npos);
    }

    // Test case for MouseScrolledEvent
    TEST_F(RZEventTests, MouseScrolledEvent)
    {
        f32 xOffset = 1.0f, yOffset = -2.5f;
        MouseScrolledEvent event(xOffset, yOffset);
        
        EXPECT_EQ(event.GetEventType(), EventType::kMouseScrolled);
        EXPECT_TRUE(event.IsInCategory(EventCategory::kMouse));
        EXPECT_TRUE(event.IsInCategory(EventCategory::kInput));
        
        EXPECT_FLOAT_EQ(event.GetXOffset(), xOffset);
        EXPECT_FLOAT_EQ(event.GetYOffset(), yOffset);
        
        // Test string representation
        std::string str = event.ToString();
        EXPECT_TRUE(str.find("MouseScrolled") != std::string::npos);
    }

    // Test case for Application Events
    TEST_F(RZEventTests, ApplicationEvents)
    {
        // Test AppTickEvent
        AppTickEvent tickEvent;
        EXPECT_EQ(tickEvent.GetEventType(), EventType::kAppTick);
        EXPECT_TRUE(tickEvent.IsInCategory(EventCategory::kApplication));
        
        // Test AppUpdateEvent
        AppUpdateEvent updateEvent;
        EXPECT_EQ(updateEvent.GetEventType(), EventType::kAppUpdate);
        EXPECT_TRUE(updateEvent.IsInCategory(EventCategory::kApplication));
        
        // Test AppRenderEvent
        AppRenderEvent renderEvent;
        EXPECT_EQ(renderEvent.GetEventType(), EventType::kAppRender);
        EXPECT_TRUE(renderEvent.IsInCategory(EventCategory::kApplication));
    }

    // Test case for Event handling flag
    TEST_F(RZEventTests, EventHandling)
    {
        KeyPressedEvent event(Keys::KEY_A, 1);
        
        // Initially not handled
        EXPECT_FALSE(event.IsHandled());
        
        // Mark as handled
        event.SetHandled(true);
        EXPECT_TRUE(event.IsHandled());
        
        // Unmark
        event.SetHandled(false);
        EXPECT_FALSE(event.IsHandled());
    }

    // Test case for EventDispatcher functionality
    TEST_F(RZEventTests, EventDispatcher)
    {
        KeyPressedEvent event(Keys::KEY_A, 1);
        EventDispatcher dispatcher(event);
        
        bool handlerCalled = false;
        auto handler = [&handlerCalled](KeyPressedEvent& e) -> bool {
            handlerCalled = true;
            return true;
        };
        
        // Dispatch the event
        dispatcher.Dispatch<KeyPressedEvent>(handler);
        
        // Verify handler was called and event was marked as handled
        EXPECT_TRUE(handlerCalled);
        EXPECT_TRUE(event.IsHandled());
    }

}    // namespace Razix