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

        void OnUpdate() override;
        void OnEvent(Event& event) override;
    private:
            
    };

}


