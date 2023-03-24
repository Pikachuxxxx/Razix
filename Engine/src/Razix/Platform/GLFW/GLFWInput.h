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

        std::pair<f32, f32> GetMousePositionImpl() override;
        f32                   GetMouseXImpl() override;
        f32                   GetMouseYImpl() override;
    };

}    // namespace Razix
