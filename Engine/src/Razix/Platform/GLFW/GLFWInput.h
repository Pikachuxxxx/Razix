#pragma once

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    class RAZIX_API GLFWInput : public RZInput
    {
    public:
        static void SelectGLFWInputManager();

    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsKeyReleasedImpl(int keycode) override;
        bool IsIsKeyHeldImpl(int keycode) override;

        bool IsMouseButtonPressedImpl(int button) override;
        bool IsMouseButtonReleasedImpl(int button) override;
        bool IsMouseButtonHeldImpl(int button) override;

        std::pair<float, float> GetMousePositionImpl() override;
        float                   GetMouseXImpl() override;
        float                   GetMouseYImpl() override;
    };

}    // namespace Razix
