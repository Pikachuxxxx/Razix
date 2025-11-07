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

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "MouseMovedEvent: %.2f, %.2f", m_MouseX, m_MouseY);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kMouseMoved)
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

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "MouseScrolledEvent: %.2f, %.2f", GetXOffset(), GetYOffset());
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kMouseScrolled)
    private:
        f32 m_XOffset, m_YOffset;
    };

    class RAZIX_API RZMouseButtonEvent : public RZEvent
    {
    public:
        inline int GetMouseButton() const { return m_Button; }

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

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "MouseButtonPressedEvent: %d", m_Button);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kMouseButtonPressed)
    };

    class RAZIX_API RZMouseButtonReleasedEvent : public RZMouseButtonEvent
    {
    public:
        RZMouseButtonReleasedEvent(int button)
            : RZMouseButtonEvent(button) {}

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "MouseButtonReleasedEvent: %d", m_Button);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kMouseButtonReleased)
    };

}    // namespace Razix