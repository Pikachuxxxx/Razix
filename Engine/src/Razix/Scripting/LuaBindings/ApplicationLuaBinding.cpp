// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindApplicationAPI()
        {
            sol::usertype<RZApplication> appType = m_State.new_usertype<RZApplication>("RZApp");

            appType.set_function("GetWindowSize", &RZApplication::getWindowSize);
            m_State.set_function("GetAppInstance", &RZApplication::Get);

            sol::table graphics = m_State.create_table("RZGraphicsContext");

            graphics.new_enum("RenderAPI", "OpenGL", Graphics::RenderAPI::OPENGL, "Vulkan", Graphics::RenderAPI::OPENGL);

            graphics.set_function("SetRenderAPI", &Graphics::RZGraphicsContext::SetRenderAPI);
            graphics.set_function("GetRenderAPIString", &Graphics::RZGraphicsContext::GetRenderAPIString);
        }

    }    // namespace Scripting
}    // namespace Razix