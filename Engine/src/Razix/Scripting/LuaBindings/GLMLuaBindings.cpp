// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindglm()
        {
            m_State.new_usertype<glm::vec2>("vec2",
                sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
                "x",
                &glm::vec2::x,
                "y",
                &glm::vec2::y);

            // https://github.com/ThePhD/sol2/issues/547
            //auto vec3_mult_overloads = sol::overload(
            //    [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1 * v2; },
            //    [](const glm::vec3& v1, float f) -> glm::vec3 { return v1 * f; },
            //    [](float f, const glm::vec3& v1) -> glm::vec3 { return f * v1; });

            m_State.new_usertype<glm::vec3>("vec3",
                sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                "x",
                &glm::vec3::x,
                "y",
                &glm::vec3::y,
                "z",
                &glm::vec3::z);

            m_State.new_usertype<glm::vec4>("vec4",
                sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>(),
                "x",
                &glm::vec4::x,
                "y",
                &glm::vec4::y,
                "z",
                &glm::vec4::z,
                "w",
                &glm::vec4::w);
        }

    }    // namespace Scripting
}    // namespace Razix