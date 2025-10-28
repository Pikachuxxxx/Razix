#pragma once

#include "Razix/Core/Utils/RZVendorOverrides.h"

#include <cereal/archives/json.hpp>

#include "Razix/Scene/Components/ActiveComponent.h"
#include "Razix/Scene/Components/CameraComponent.h"
#include "Razix/Scene/Components/HierarchyComponent.h"
#include "Razix/Scene/Components/IDComponent.h"
#include "Razix/Scene/Components/LightComponent.h"
#include "Razix/Scene/Components/LuaScriptComponent.h"
#include "Razix/Scene/Components/MeshRendererComponent.h"
#include "Razix/Scene/Components/TagComponent.h"
#include "Razix/Scene/Components/TransformComponent.h"

/**
* Components are various classes that are added to the entities to provide functionality in a decoupled way
* They have no info about entities at all
*/

// Register List of all components that razix implements that is used while serialization adn with entt
#define RAZIX_COMPONENTS IDComponent, TagComponent, ActiveComponent, TransformComponent, CameraComponent, MeshRendererComponent, LuaScriptComponent, LightComponent, HierarchyComponent