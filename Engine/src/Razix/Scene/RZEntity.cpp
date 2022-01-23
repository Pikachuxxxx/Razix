#include "rzxpch.h"
#include "RZEntity.h"

namespace Razix {

    RZEntity::RZEntity(entt::entity handle, RZScene* scene)
        : m_Entity(handle), m_Scene(scene) { }
}