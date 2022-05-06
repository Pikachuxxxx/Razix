#pragma once

#include "Razix/Core/RZCore.h"

#include <functional>
#include <iostream>
#include <sstream>

namespace Razix {

    // Events in Razix are currently blocking
    //
    // This means that they are handled/dispatched as given and not queued
    enum class EventType
    {
        None = 0,
        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,
        AppTick,
        AppUpdate,
        AppRender,
        KeyPressed,
        KeyReleased,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    enum class EventCategory
    {
        None                     = 0,
        EventCategoryApplication = RZ_BIT_SHIFT (0),
        EventCategoryInput       = RZ_BIT_SHIFT (1),
        EventCategoryKeyboard    = RZ_BIT_SHIFT (2),
        EventCategoryMouse       = RZ_BIT_SHIFT (3),
        EventCategoryMouseButton = RZ_BIT_SHIFT (4)
    };

#define EVENT_CLASS_TYPE(type)                                                      \
    static EventType    GetStaticType () { return EventType::##type; }              \
    virtual EventType   GetEventType () const override { return GetStaticType (); } \
    virtual const char* GetName () const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags () const override { return category; }

    class RAZIX_API RZEvent
    {
        friend class RZEventDispatcher;

    public:
        bool Handled = false;

    public:
        virtual EventType   GetEventType () const     = 0;
        virtual const char* GetName () const          = 0;
        virtual int         GetCategoryFlags () const = 0;
        virtual std::string ToString () const { return GetName (); }

        inline bool IsInCategory (EventCategory category)
        {
            return GetCategoryFlags () & (int) category;
        }
    };

    class RZEventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool (T&)>;

    public:
        RZEventDispatcher (RZEvent& event)
            : m_Event (event)
        {
        }

        template<typename T>
        bool Dispatch (EventFn<T> func)
        {
            if (m_Event.GetEventType () == T::GetStaticType ()) {
                m_Event.Handled = func (*(T*) &m_Event);
                return true;
            }
            return false;
        }

    private:
        RZEvent& m_Event;
    };

    inline std::ostream& operator<< (std::ostream& os, const RZEvent& e)
    {
        return os << e.ToString ();
    }
}    // namespace Razix