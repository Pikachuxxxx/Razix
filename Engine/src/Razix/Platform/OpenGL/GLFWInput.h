#pragma once

#include "Razix/Core/OS/RazixInput.h"

namespace Razix
{
    class RAZIX_API GLFWInput : public RazixInput
    {
    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsKeyReleasedImpl(int keycode) override;
        bool IsIsKeyHeldImpl(int keycode) override;

        bool IsMouseButtonPressedImpl(int button) override;
        std::pair<float, float> GetMousePositionImpl() override;
        float GetMouseXImpl() override;
        float GetMouseYImpl() override;

    };

}

