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
            
        }
    }    // namespace Scripting
}    // namespace Razix