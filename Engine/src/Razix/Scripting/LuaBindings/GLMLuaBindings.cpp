// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Razix {
    namespace Scripting {

        // https://github.com/ThePhD/sol2/issues/620
        static std::string SOL_VEC3_TOSTRING(const glm::vec3& v)
        {
            std::string s = "vec3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
            return s;
        }

        void RZLuaScriptHandler::bindglm()
        {
            sol::table globals = m_State.globals();
            sol::table rzmath  = m_State.create_table();
            globals["glm"]    = rzmath;

            rzmath.new_usertype<glm::vec2>("vec2",
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

            rzmath.new_usertype<glm::vec3>("vec3",
                sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                "x",
                &glm::vec3::x,
                "y",
                &glm::vec3::y,
                "z",
                &glm::vec3::z,
                sol::meta_function::to_string,
                &SOL_VEC3_TOSTRING);

            rzmath.new_usertype<glm::vec4>("vec4",
                sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>(),
                "x",
                &glm::vec4::x,
                "y",
                &glm::vec4::y,
                "z",
                &glm::vec4::z,
                "w",
                &glm::vec4::w);

            //rzmath.new_usertype<glm::quat>("quat",
            //    sol::constructors<glm::quat(), glm::quat(float), glm::quat(float, float, float,float)>(),
            //    "x",
            //    &glm::quat::x,
            //    "y",
            //    &glm::quat::y,
            //    "z",
            //    &glm::quat::z,
            //    "w",
            //    &glm::quat::w);
        }
    }    // namespace Scripting
}    // namespace Razix