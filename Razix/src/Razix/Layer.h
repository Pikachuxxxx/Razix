#pragma once

#include "Razix/Core.h"
#include "Razix/Events/Event.h"

namespace Razix
{
    class RAZIX_API Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();
        
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event& event) {}

#ifndef RZX_DIST
        inline const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
#endif
    };
}

