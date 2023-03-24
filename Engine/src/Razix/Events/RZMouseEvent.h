#pragma once

#include "RZEvent.h"

namespace Razix {

    class RAZIX_API RZMouseMovedEvent : public RZEvent
    {
    public:
        RZMouseMovedEvent(f32 x, f32 y)
            : m_MouseX(x), m_MouseY(y) {}

        inline f32 GetX() const { return m_MouseX; }
        inline f32 GetY() const { return m_MouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryMouse | (int) EventCategory::EventCategoryInput)
    private:
        f32 m_MouseX, m_MouseY;
    };

    class RAZIX_API RZMouseScrolledEvent : public RZEvent
    {
    public:
        RZMouseScrolledEvent(f32 xOffset, f32 yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) {}

        inline f32 GetXOffset() const { return m_XOffset; }
        inline f32 GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryMouse | (int) EventCategory::EventCategoryInput)
    private:
        f32 m_XOffset, m_YOffset;
    };

    class RAZIX_API RZMouseButtonEvent : public RZEvent
    {
    public:
        inline int GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryMouse | (int) EventCategory::EventCategoryInput)
    protected:
        RZMouseButtonEvent(int button)
            : m_Button(button) {}

        int m_Button;
    };

    class RAZIX_API RZMouseButtonPressedEvent : public RZMouseButtonEvent
    {
    public:
        RZMouseButtonPressedEvent(int button)
            : RZMouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class RAZIX_API RZMouseButtonReleasedEvent : public RZMouseButtonEvent
    {
    public:
        RZMouseButtonReleasedEvent(int button)
            : RZMouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

}    // namespace Razix
