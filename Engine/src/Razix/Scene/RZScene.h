#pragma once

#include "Razix/Core/RZCore.h"

#include <entt.hpp>

namespace Razix {

    class RZEntity;

    /* Scene contains entities that will be used by the engine for rendering and other runtime systems */
    class RAZIX_API RZScene
    {
    public:
        RZScene() = default;
        RZScene(std::string sceneName);

        RZEntity createEntity(const std::string& name = std::string());
        void destroyEntity(RZEntity entity);

        RAZIX_INLINE const std::string& getSceneName() const { return m_SceneName; }

    private:
        entt::registry m_Registry;
        std::string m_SceneName = "default";

        friend class RZEntity;
    };
}