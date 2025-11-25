#pragma once

#include "RZEvent.h"

namespace Razix {

    class RAZIX_API RZKeyEvent : public RZEvent
    {
    public:
        inline int GetKeyCode() const { return m_KeyCode; }

    protected:
        RZKeyEvent(int keycode)
            : m_KeyCode(keycode) {}

        int m_KeyCode;
    };

    class RAZIX_API RZKeyPressedEvent : public RZKeyEvent
    {
    public:
        RZKeyPressedEvent(int keycode, int repeatCount)
            : RZKeyEvent(keycode), m_RepeatCount(repeatCount) {}

        inline int GetRepeatCount() const { return m_RepeatCount; }

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "KeyPressedEvent: %d ( %d repeats)", m_KeyCode, m_RepeatCount);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kKeyPressed)
    private:
        int m_RepeatCount;
    };

    class RAZIX_API RZKeyReleasedEvent : public RZKeyEvent
    {
    public:
        RZKeyReleasedEvent(int keycode)
            : RZKeyEvent(keycode) {}

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "KeyReleasedEvent: %d ", m_KeyCode);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kKeyReleased)
    };
}    // namespace Razix
