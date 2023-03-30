// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include "Razix/Graphics/Renderers/RZDebugRenderer.h"

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindRendererAPI()
        {
            //sol::usertype<Graphics::RZDebugRenderer> debugDrawType = m_State.new_usertype<Graphics::RZDebugRenderer>("DebugDraw");

            sol::table globals    = m_State.globals();
            sol::table debug_draw = m_State.create_table();
            globals["DebugDraw"]  = debug_draw;

            debug_draw["DrawPoint"] = [](const glm::vec3& pos, f32 point_radius, const glm::vec3& colour) { Graphics::RZDebugRenderer::DrawPoint(pos, point_radius, colour); };
            debug_draw["DrawPointAlpha"] = [](const glm::vec3& pos, f32 point_radius, const glm::vec4& colour) { Graphics::RZDebugRenderer::DrawPoint(pos, point_radius, colour); };

            //sol::overload(
            //    [](const glm::vec3& pos, f32 point_radius, const glm::vec3& colour) {
            //
            //    },
            //    [](const glm::vec3& pos, f32 point_radius, const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {
            //        Graphics::RZDebugRenderer::DrawPoint(pos, point_radius, colour);
            //    });
        }

    }    // namespace Scripting
}    // namespace Razix