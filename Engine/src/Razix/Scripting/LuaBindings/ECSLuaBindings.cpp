// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Scene/RZEntity.h"

#include "Razix/Scene/Components/RZComponents.h"

/** Ex. https://sol2.readthedocs.io/en/latest/api/usertype.html
	lua.new_usertype<object>( "object" );

	// runtime additions: through the sol API
	lua["object"]["func"] = [](object& o) { return o.value; };
	// runtime additions: through a lua script
	lua.script("function object:print () print(self:func()) end");
	
	// see it work
	lua.script("local obj = object.new() \n obj:print()");
 */

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindECSAPI()
        {
            sol::table globals = m_State.globals();
            sol::table ecs     = m_State.create_table();
            globals["ECS"]     = ecs;

            sol::usertype<RZEntity> entity = ecs.new_usertype<RZEntity>("RZEntity");

            // Register components as types
            sol::usertype<TransformComponent> transformComponentType = m_State.new_usertype<TransformComponent>("TransformComponent");

            ecs["getEntityByName"] = [] {
                //RZSceneManager::Get().getCurrentScene()->getRegistry().fin
            };
            //ecs["getTransfromComponent"];
            //ecs["setTransfromComponent"];

            entity["getPosition"] = [](Razix::RZEntity& e) {
                return e.GetComponent<TransformComponent>().Translation;
            };
            entity["getRotation"] = [](Razix::RZEntity& e) {
                return e.GetComponent<TransformComponent>().Rotation;
            };
            entity["getScale"] = [](Razix::RZEntity& e) {
                return e.GetComponent<TransformComponent>().Scale;
            };

            entity["setPosition"] = [](Razix::RZEntity& e, glm::vec3 pos) {
                return e.GetComponent<TransformComponent>().Translation = pos;
            };
            entity["setRotation"];
            entity["setScale"];

            ecs["getCameraComponent"];

            ecs["getCameraPosition"];
            ecs["getCameraRotation"];

            ecs["setCameraPosition"];
            ecs["setCameraRotation"];
        }
    }    // namespace Scripting
}    // namespace Razix