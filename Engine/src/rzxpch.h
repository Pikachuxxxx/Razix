#pragma once

//-----C Standard Includes-----//
#include <stdint.h>

//-----System Includes-----//
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <type_traits>
#include <utility>

//-----Data Structures-----//
// TODO: DIsable this once rzstl replacement is done
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

//-----Engine-----//
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZDebugConfig.h"
#include "Razix/Core/RZLog.h"
#include "Razix/Core/RZProfiling.h"
#include "Razix/Core/RZRoot.h"
#include "Razix/Graphics/RHI/API/RZAPIDesc.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"
#include "Razix/Graphics/Resources/IRZResource.h"

//-----Engine Internal-----//
// Razix Memory
#include "internal/RazixMemory/include/RZMemory.h"
// Razix STL
#include "internal/RZSTL/include/RZSTL/rzstl.h"

//-----3rd Party-----//
// Profilers
#include <Tracy.hpp>
// SOL
#include <sol/sol.hpp>
// GLM
#include <glm/glm.hpp>
// Cereal
#include <cereal/cereal.hpp>

// Physics Engine
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your pre compiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

//-----Graphics-----//
#ifdef RAZIX_RENDER_API_VULKAN
    #include <vma/vk_mem_alloc.h>
    #include <vulkan/vulkan.h>
#endif

// Disable Warnings
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)    // some spdlog stuff
