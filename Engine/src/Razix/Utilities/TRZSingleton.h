#pragma once

#include "Razix/Core/RZCore.h"
/**
 * When we define a template function or class, the compiler only generates code for it when it's actually used.
 * That works fine when everything is in a header file, but if we move the definition to a `.cpp` file,
 * the linker needs to know where to find it. Otherwise, we get those annoying "undefined symbol" errors.
 *
 * To fix that, we explicitly instantiate the template in the `.cpp` file like this:
 *
 *     template class RZSingleton<RZResourceManager>;
 *
 * This tells the compiler, "Hey, generate the code for `RZSingleton<RZResourceManager>` right here."
 * That way, any translation unit that includes the header can just link against this definition instead of
 * trying to create its own.
 *
 * This is especially important when working with DLLs because if we don’t do this,
 * both the EXE and DLL will end up creating their own separate instances of `RZSingleton<RZResourceManager>`,
 * which totally breaks the whole singleton pattern. Which is why we hide the Get implementation in .cpp and define
 * a template class RZSingleton<T>; for each type of the singleton we use to maintain a single copy.
 * This is a bit of manual work at the cost of functionality which is fine.
 */

namespace Razix {
    /**
     * A Singleton type class template
     * @brief We return a static const reference instead of a lazy heap creation
     * @note [Design Suggested in Game Engine Architecture by Jason Gregory at p.199-201]
     */
    template<class T>
    class RAZIX_API RZSingleton
    {
    public:
        /* Gets the reference to the Singleton Object */
        static T& Get();
        
    protected:
        RZSingleton() {}
        ~RZSingleton() {}

    private:
        RAZIX_NONCOPYABLE_CLASS(RZSingleton);
    };

#pragma warning(disable : 4661)

    // TODO: Test with DCLP before using
    // TODO: Do not use this yet!
    template<class T>
    class RZThreadSafeSingleton
    {
    public:
        static T& Get()
        {
            std::lock_guard<std::mutex> lock(m_mConstructed);    //Lock is required here though, to prevent multiple threads initializing multiple instances of the class when it turns out it has not been initialized yet
            static T                    instance;
            return instance;
        }

    protected:
        //Only allow the class to be created and destroyed by itself
        RZThreadSafeSingleton() {}
        ~RZThreadSafeSingleton() {}

        static std::mutex m_mConstructed;

    private:
        RAZIX_NONCOPYABLE_CLASS(RZThreadSafeSingleton);
    };

    template<class T>
    std::mutex Razix::RZThreadSafeSingleton<T>::m_mConstructed;

}    // namespace Razix
