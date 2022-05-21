#pragma once
#include "RZEvent.h"

namespace Razix {

    class RAZIX_API RZWindowResizeEvent : public RZRoot, public RZEvent
    {
    public:
        RZWindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        inline unsigned int GetWidth() const { return m_Width; }
        inline unsigned int GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryApplication)
    private:
        unsigned int m_Width, m_Height;
    };

    class RAZIX_API WindowCloseEvent : public RZEvent
    {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryApplication)
    };

    class RAZIX_API AppTickEvent : public RZEvent
    {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryApplication)
    };

    class RAZIX_API AppUpdateEvent : public RZEvent
    {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryApplication)
    };

    class RAZIX_API AppRenderEvent : public RZEvent
    {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY((int) EventCategory::EventCategoryApplication)
    };
}    // namespace Razix