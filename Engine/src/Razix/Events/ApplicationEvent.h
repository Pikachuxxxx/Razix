#pragma once
#include "RZEvent.h"

namespace Razix {

    class RAZIX_API RZWindowResizeEvent : public RZEvent
    {
    public:
        RZWindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        inline unsigned int GetWidth() const { return m_Width; }
        inline unsigned int GetHeight() const { return m_Height; }

        RZString ToString() const override
        {
            char buffer[256];
            rz_snprintf(buffer, sizeof(buffer), "WindowResizeEvent: (%d, %d)", m_Width, m_Height);
            return RZString(buffer);
        }

        EVENT_CLASS_TYPE(kWindowResize)
    private:
        unsigned int m_Width, m_Height;
    };

    class RAZIX_API WindowCloseEvent : public RZEvent
    {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(kWindowClose)
    };

    class RAZIX_API AppTickEvent : public RZEvent
    {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(kAppTick)
    };

    class RAZIX_API AppUpdateEvent : public RZEvent
    {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(kAppUpdate)
    };

    class RAZIX_API AppRenderEvent : public RZEvent
    {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(kAppRender)
    };
}    // namespace Razix
