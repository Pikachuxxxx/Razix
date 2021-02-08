#pragma once

#include "Razix/Core.h"

namespace Razix
{
    class RAZIX_API Input
    {
    public:
        inline static bool IsKeyPressed(int keycode) { return sInstance->IsKeyPressedImpl(keycode); }
        inline static bool IsKeyReleased(int keycode) { return sInstance->IsKeyReleasedImpl(keycode); }
        inline static bool IsKeyHeld(int keycode) { return sInstance->IsIsKeyHeldImpl(keycode); }

        inline static bool IsMouseButtonPressed(int button) { return sInstance->IsMouseButtonPressedImpl(button); }
        inline static std::pair<float, float> GetMousePosition() { return sInstance->GetMousePosition(); }
        inline static float GetMouseX() { return sInstance->GetMouseXImpl(); }
        inline static float GetMouseY() { return sInstance->GetMouseYImpl(); }
    protected:
        virtual bool IsKeyPressedImpl(int keycode) = 0;
        virtual bool IsKeyReleasedImpl(int keycode) = 0;
        virtual bool IsIsKeyHeldImpl(int keycode) = 0;

        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual std::pair<float, float> GetMousePositionImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;
    private:
        static Input* sInstance;
    };
}
