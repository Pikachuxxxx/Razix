#pragma once

//-----C Standard Includes-----//
#include <stdint.h>

//-----System Includes-----//
#ifdef __cplusplus
    #include <algorithm>
    #include <chrono>
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
    #include <variant>

    //-----Data Structures-----//
    // TODO: DIsable this once rzstl replacement is done
    #include <map>
    #include <memory>
    #include <set>
    #include <stack>
    #include <string>
    #include <unordered_map>
    #include <unordered_set>
    #include <vector>

    //-----Engine-----//
    #include "Razix/Core//Profiling/RZProfiling.h"
    #include "Razix/Core/Log/RZLog.h"
    #include "Razix/Core/RZCore.h"
    #include "Razix/Core/RZDataTypes.h"
    #include "Razix/Core/RZDebugConfig.h"
    #include "Razix/Core/RZRoot.h"
    #include "Razix/Gfx/RHI/API/RZAPIDesc.h"
    #include "Razix/Gfx/RHI/API/RZAPIHandles.h"
    #include "Razix/Gfx/Resources/IRZResource.h"

    //-----Engine Internal-----//
    // Razix Memory
    #include "internal/RazixMemory/include/RZMemory.h"
    // Razix STL
    #include "internal/RZSTL/include/RZSTL/rzstl.h"

    //-----3rd Party-----//
    // Profilers
    #include <Tracy.hpp>
    // GLM
    #define GLM_FORCE_LEFT_HANDED
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
        #if RAZIX_USE_VMA
            #include <vma/vk_mem_alloc.h>
        #endif
        #include <vulkan/vulkan.h>
    #endif

    #ifdef RAZIX_RENDER_API_DIRECTX12
        #include <d3d12.h>
    #endif

    // Disable Warnings
    #pragma warning(disable : 4100)
    #pragma warning(disable : 4996)    // some spdlog stuff
#endif
