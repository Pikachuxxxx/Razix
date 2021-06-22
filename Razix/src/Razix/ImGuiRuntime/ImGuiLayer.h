#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Layer.h"

namespace Razix
{

    class RAZIX_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImguiRender() override;

        void Begin();
        void End();
    private:
        float m_Time = 0.0f;
    };

}


