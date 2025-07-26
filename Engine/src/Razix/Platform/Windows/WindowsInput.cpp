// clang-format off
#include "rzxpch.h"
// clang-format on
#include "WindowsInput.h"

#include "Razix/Core/App/RZApplication.h"
#include <map>

// Windows-specific includes
#ifdef RAZIX_PLATFORM_WINDOWS
    #include <Windows.h>
    #include <Xinput.h>
    #pragma comment(lib, "Xinput.lib")
#endif

namespace Razix {

#ifdef RAZIX_PLATFORM_WINDOWS

    HWND WindowsInput::GetActiveWindow()
    {
        return static_cast<HWND>(RZApplication::Get().getWindow()->GetNativeWindow());
    }

    // Keyboard implementation
    bool WindowsInput::IsKeyPressedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Check if key is currently pressed
        SHORT keyState = GetAsyncKeyState(keycode);
        return (keyState & 0x8000) != 0;
    }

    bool WindowsInput::IsKeyReleasedImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Check if key was pressed since last call but is not currently pressed
        SHORT keyState = GetAsyncKeyState(keycode);
        return (keyState & 0x0001) != 0 && (keyState & 0x8000) == 0;
    }

    bool WindowsInput::IsIsKeyHeldImpl(int keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Same as IsKeyPressed for Windows
        return IsKeyPressedImpl(keycode);
    }

    // Mouse implementation
    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (button) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        static std::map<int, bool> previousStates;
        bool currentState = (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
        bool wasPressed = previousStates[button];
        previousStates[button] = currentState;
        
        return currentState && !wasPressed;
    }

    bool WindowsInput::IsMouseButtonReleasedImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (button) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        static std::map<int, bool> previousStates;
        bool currentState = (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
        bool wasPressed = previousStates[button];
        previousStates[button] = currentState;
        
        return !currentState && wasPressed;
    }

    bool WindowsInput::IsMouseButtonHeldImpl(int button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (button) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        return (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
    }

    std::pair<f32, f32> WindowsInput::GetMousePositionImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        POINT point;
        GetCursorPos(&point);
        
        // Convert screen coordinates to client coordinates
        HWND hwnd = GetActiveWindow();
        ScreenToClient(hwnd, &point);
        
        return {static_cast<f32>(point.x), static_cast<f32>(point.y)};
    }

    f32 WindowsInput::GetMouseXImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    f32 WindowsInput::GetMouseYImpl()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

    // Gamepad implementation using XInput
    bool WindowsInput::IsGamepadConnected()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        // Check if controller 0 is connected
        DWORD result = XInputGetState(0, &state);
        return result == ERROR_SUCCESS;
    }

    f32 WindowsInput::GetJoyLeftStickHorizontalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            SHORT stick = state.Gamepad.sThumbLX;
            // Normalize to -1.0 to 1.0 range
            return static_cast<f32>(stick) / 32767.0f;
        }
        return 0.0f;
    }

    f32 WindowsInput::GetJoyLeftStickVerticalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            SHORT stick = state.Gamepad.sThumbLY;
            // Normalize to -1.0 to 1.0 range
            return static_cast<f32>(stick) / 32767.0f;
        }
        return 0.0f;
    }

    f32 WindowsInput::GetJoyRightStickHorizontalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            SHORT stick = state.Gamepad.sThumbRX;
            // Normalize to -1.0 to 1.0 range
            return static_cast<f32>(stick) / 32767.0f;
        }
        return 0.0f;
    }

    f32 WindowsInput::GetJoyRightStickVerticalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            SHORT stick = state.Gamepad.sThumbRY;
            // Normalize to -1.0 to 1.0 range
            return static_cast<f32>(stick) / 32767.0f;
        }
        return 0.0f;
    }

    f32 WindowsInput::GetJoyDPadHorizontalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            WORD buttons = state.Gamepad.wButtons;
            f32 horizontal = 0.0f;
            
            if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) horizontal -= 1.0f;
            if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) horizontal += 1.0f;
            
            return horizontal;
        }
        return 0.0f;
    }

    f32 WindowsInput::GetJoyDPadVerticalImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            WORD buttons = state.Gamepad.wButtons;
            f32 vertical = 0.0f;
            
            if (buttons & XINPUT_GAMEPAD_DPAD_UP) vertical -= 1.0f;
            if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) vertical += 1.0f;
            
            return vertical;
        }
        return 0.0f;
    }

    bool WindowsInput::IsCrossPressedImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
        }
        return false;
    }

    bool WindowsInput::IsCirclePressedImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
        }
        return false;
    }

    bool WindowsInput::IsTrianglePressedImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
        }
        return false;
    }

    bool WindowsInput::IsSquarePressedImpl()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
        }
        return false;
    }

#else
    // Stub implementations for non-Windows platforms
    HWND WindowsInput::GetActiveWindow() { return nullptr; }
    bool WindowsInput::IsKeyPressedImpl(int keycode) { return false; }
    bool WindowsInput::IsKeyReleasedImpl(int keycode) { return false; }
    bool WindowsInput::IsIsKeyHeldImpl(int keycode) { return false; }
    bool WindowsInput::IsMouseButtonPressedImpl(int button) { return false; }
    bool WindowsInput::IsMouseButtonReleasedImpl(int button) { return false; }
    bool WindowsInput::IsMouseButtonHeldImpl(int button) { return false; }
    std::pair<f32, f32> WindowsInput::GetMousePositionImpl() { return {0.0f, 0.0f}; }
    f32 WindowsInput::GetMouseXImpl() { return 0.0f; }
    f32 WindowsInput::GetMouseYImpl() { return 0.0f; }
    bool WindowsInput::IsGamepadConnectedImpl() { return false; }
    f32 WindowsInput::GetJoyLeftStickHorizontalImpl() { return 0.0f; }
    f32 WindowsInput::GetJoyLeftStickVerticalImpl() { return 0.0f; }
    f32 WindowsInput::GetJoyRightStickHorizontalImpl() { return 0.0f; }
    f32 WindowsInput::GetJoyRightStickVerticalImpl() { return 0.0f; }
    f32 WindowsInput::GetJoyDPadHorizontalImpl() { return 0.0f; }
    f32 WindowsInput::GetJoyDPadVerticalImpl() { return 0.0f; }
    bool WindowsInput::IsCrossPressedImpl() { return false; }
    bool WindowsInput::IsCirclePressedImpl() { return false; }
    bool WindowsInput::IsTrianglePressedImpl() { return false; }
    bool WindowsInput::IsSquarePressedImpl() { return false; }
#endif

}