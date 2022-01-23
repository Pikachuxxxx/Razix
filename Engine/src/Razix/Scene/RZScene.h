#pragma once

#include <entt.hpp>

namespace Razix {
    /* Scene contains entities that will be used by the engine for rendering and other runtime systems */
    class RZScene
    {
    public:
        RZScene();
        ~RZScene();

    private:
        entt::registry m_Registry;
    };
}