#pragma once

#include "Razix/Core.h"
#include "Razix/Layer.h"

namespace Razix
{

    class RAZIX_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnUpdate() override;
        virtual void OnEvent(Event& event) override;
    private:
            
    };

}


