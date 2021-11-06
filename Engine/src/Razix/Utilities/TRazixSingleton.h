#pragma once

#include "Razix/Core/Core.h"

namespace Razix
{
    /**
     * A Singleton type class template
     * @brief We return a static const reference instead of a lazy heap creation
     * @note [Design Suggested in Game Engine Architecture by Jason Gregory at p.199-201]
     */
    template <class T>
    class RZSingleton
    {
    public:
        /* Gets the reference to the Singleton Object */
        static T& Get()
        {
            static T instance;
            return instance;
        }
    protected:
        RZSingleton() {}
        ~RZSingleton() {}
    private:
        ///	Make the singleton non-copyable
        NONCOPYABLE(RZSingleton);
    };

    /*
    // TODO: Test with DCLP before using
    // TODO: Do not use this yet!
    template <class T>
    class RZThreadSafeSingleton
    {
    public:
        static T& Get() {
            std::lock_guard<std::mutex> lock(m_mConstructed); //Lock is required here though, to prevent multiple threads initializing multiple instances of the class when it turns out it has not been initialized yet
            static RZSingleton<T> instance;
            return instance;
        }

    protected:
        //Only allow the class to be created and destroyed by itself
        RZThreadSafeSingleton() {}
        ~RZThreadSafeSingleton() {}

        static std::mutex m_mConstructed;

    private:
        NONCOPYABLE(RZThreadSafeSingleton);
    };
    */
}