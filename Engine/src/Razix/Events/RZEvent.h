#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Core/std/utility.h"

#include <functional>
#include <sstream>
#include <typeindex>
#include <unordered_map>

namespace Razix {

    // TODO: use macros for new event type/category registration and fill these enums using macros during compile time
    // https://deplinenoise.wordpress.com/2014/02/23/using-c11-capturing-lambdas-w-vanilla-c-api-functions/
    // Events in Razix are currently blocking
    // This means that they are handled/dispatched as given and not queued
    enum class EventType
    {
        kNone = 0,
        // Window
        kWindowClose,
        kWindowResize,
        kWindowFocus,
        kWindowLostFocus,
        kWindowMoved,
        // App
        kAppTick,
        kAppUpdate,
        kAppRender,
        // Input
        kKeyPressed,
        kKeyReleased,
        kMouseButtonPressed,
        kMouseButtonReleased,
        kMouseMoved,
        kMouseScrolled,
        // TODO: Joystick events
        // Asset System
        kAssetCreated,
        kAssetModified,
        kAssetMemoryMoved,
        kAssetCopied,
        kAssetDeleted,
        kAssetBroken,
        kAssetSerialized,
        kAssetDeserialzed,
        COUNT
    };

    // util macros to simplify code gen
#define EVENT_CLASS_TYPE(type)                      \
    static EventType GetStaticType()                \
    {                                               \
        return EventType::type;                     \
    }                                               \
    virtual EventType GetEventType() const override \
    {                                               \
        return GetStaticType();                     \
    }                                               \
    virtual cstr GetName() const override           \
    {                                               \
        return #type;                               \
    }

    /**
     * Base class from which all the RZEvents derive
     * This class uses visitor pattern
     */
    class RAZIX_API RZEvent
    {
    public:
        bool Handled = false;

    public:
        virtual ~RZEvent() = default;

        virtual cstr        GetName() const      = 0;
        virtual EventType   GetEventType() const = 0;
        virtual std::string ToString() const { return GetName(); }

    private:
        friend class RZEventDispatcher;
    };

    /**
     * Event dispatcher using callback functions
     */
    class RZEventDispatcher
    {
    public:
        template<typename T>
        void registerCallback(std::function<void(T&)> callback)
        {
            callbacks[typeid(T)] = [cb = rz_move(callback)](RZEvent& e) {
                cb(static_cast<T&>(e));
            };
        }

        void dispatch(RZEvent& event)
        {
            auto it = callbacks.find(typeid(event));
            if (it != callbacks.end()) {
                it->second(event);
            }
        }

    private:
        using Callback = std::function<void(RZEvent&)>;
        std::unordered_map<std::type_index, Callback> callbacks;
    };

}    // namespace Razix
