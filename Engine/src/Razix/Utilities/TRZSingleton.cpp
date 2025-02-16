// clang-format off
#include "rzxpch.h"
// clang-format on

#include "TRZSingleton.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Gfx/RZShaderLibrary.h"
#include "Razix/Gfx/Renderers/RZDebugRendererProxy.h"
#include "Razix/Gfx/Renderers/RZImGuiRendererProxy.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"
#include "Razix/Scene/RZSceneManager.h"
#include "Razix/Scripting/RZLuaScriptHandler.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
#endif

namespace Razix {

    template<class T>
    T& RZSingleton<T>::Get()
    {
        static T instance;
        return instance;
    };

    // create implementation for Get for the following types, unifies a single singleton instance when engine DLL is imported
    template class Razix::RZSingleton<Razix::RZEngine>;
    template class Razix::RZSingleton<Razix::RZSplashScreen>;
    template class Razix::RZSingleton<Razix::RZVirtualFileSystem>;
    template class Razix::RZSingleton<Razix::Gfx::RZResourceManager>;
    template class Razix::RZSingleton<Razix::Gfx::RZShaderLibrary>;
    template class Razix::RZSingleton<Razix::RZSceneManager>;
    template class Razix::RZSingleton<Razix::Scripting::RZLuaScriptHandler>;
    template class Razix::RZSingleton<Razix::Gfx::RZDebugRendererProxy>;
    template class Razix::RZSingleton<Razix::Gfx::RZImGuiRendererProxy>;

#ifdef RAZIX_RENDER_API_VULKAN
    template class Razix::RZSingleton<Razix::Gfx::VKDevice>;
#endif

}    // namespace Razix
