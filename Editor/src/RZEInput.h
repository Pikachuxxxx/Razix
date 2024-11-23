#if 0
#pragma once

#include "Razix/Core/RZRoot.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/OS/RZInput.h"

#include <set>

namespace Razix {
    namespace Editor {
        class RZEInput : public RZInput
        {
        public:
            RZEInput() {}
            ~RZEInput() {}

            static void SelectQTInputManager();

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
    }    // namespace Editor
}    // namespace Razix
#endif
