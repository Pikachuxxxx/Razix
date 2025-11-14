#pragma once

//-----C Standard Includes-----//
#include <stdint.h>

//-----System Includes-----//
#ifdef __cplusplus
    //-----Data Structures-----//
    #include <map>
    #include <unordered_map>
    #include <vector>
#endif

//-----Engine-----//
#include "Razix/Core//Profiling/RZProfiling.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZDebugConfig.h"
// Containers
#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/bits.h"
#include "Razix/Core/Containers/hash_functors.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/initializer_list.h"
#include "Razix/Core/Containers/queue.h"
#include "Razix/Core/Containers/ref_counter.h"
#include "Razix/Core/Containers/ring_buffer.h"
#include "Razix/Core/Containers/smart_pointers.h"
#include "Razix/Core/Containers/stack.h"
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Containers/string_utils.h"
// Std
#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/spinlock.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/std/utility.h"

// Razix Memory
#include "Razix/Core/Memory/RZMemory.h"

// Math and GLM
#include "Razix/Math/Math.h"
//-----3rd Party-----//
// Profilers
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
// GLM
//  Managed by Engine Math.h
// Cereal
#include <cereal/cereal.hpp>
// Physics Engine
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
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
    #include <volk.h>
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include <d3d12.h>
#endif

// Disable Warnings
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)    // some spdlog stuff
