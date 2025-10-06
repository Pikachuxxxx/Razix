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
    bool RZInput::IsKeyPressed(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Check if key is currently pressed
        SHORT keyState = GetAsyncKeyState(int(keycode));
        return (keyState & 0x8000) != 0;
    }

    bool RZInput::IsKeyReleased(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Check if key was pressed since last call but is not currently pressed
        SHORT keyState = GetAsyncKeyState(int(keycode));
        return (keyState & 0x0001) != 0 && (keyState & 0x8000) == 0;
    }

    bool RZInput::IsKeyHeld(Razix::KeyCode::Key keycode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Same as IsKeyPressed for Windows
        return IsKeyPressed(keycode);
    }

    // Mouse implementation
    bool RZInput::IsMouseButtonPressed(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (int(button)) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        static std::map<int, bool> previousStates;
        bool currentState = (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
        bool wasPressed = previousStates[int(button)];
        previousStates[int(button)] = currentState;
        
        return currentState && !wasPressed;
    }

    bool RZInput::IsMouseButtonReleased(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (int(button)) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        static std::map<int, bool> previousStates;
        bool currentState = (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
        bool wasPressed = previousStates[int(button)];
        previousStates[int(button)] = currentState;
        
        return !currentState && wasPressed;
    }

    bool RZInput::IsMouseButtonHeld(Razix::KeyCode::MouseKey button)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        // Map mouse button codes to Windows virtual key codes
        int windowsButton = 0;
        switch (int(button)) {
            case 0: windowsButton = VK_LBUTTON; break;  // Left mouse button
            case 1: windowsButton = VK_RBUTTON; break;  // Right mouse button
            case 2: windowsButton = VK_MBUTTON; break;  // Middle mouse button
            default: return false;
        }
        
        return (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
    }

    std::pair<f32, f32> RZInput::GetMousePosition()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        POINT point;
        GetCursorPos(&point);
        
        // Convert screen coordinates to client coordinates
        HWND hwnd = WindowsInput::GetActiveWindow();
        ScreenToClient(hwnd, &point);
        
        return {static_cast<f32>(point.x), static_cast<f32>(point.y)};
    }

    f32 RZInput::GetMouseX()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        auto [x, y] = GetMousePosition();
        return x;
    }

    f32 RZInput::GetMouseY()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        
        auto [x, y] = GetMousePosition();
        return y;
    }

    // Gamepad implementation using XInput
    bool RZInput::IsGamepadConnected()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        // Check if controller 0 is connected
        DWORD result = XInputGetState(0, &state);
        return result == ERROR_SUCCESS;
    }

    f32 RZInput::GetJoyLeftStickHorizontal()
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

    f32 RZInput::GetJoyLeftStickVertical()
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

    f32 RZInput::GetJoyRightStickHorizontal()
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

    f32 RZInput::GetJoyRightStickVertical()
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

    f32 RZInput::GetJoyDPadHorizontal()
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

    f32 RZInput::GetJoyDPadVertical()
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

    bool RZInput::IsCrossPressed()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
        }
        return false;
    }

    bool RZInput::IsCirclePressed()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
        }
        return false;
    }

    bool RZInput::IsTrianglePressed()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        
        if (XInputGetState(0, &state) == ERROR_SUCCESS) {
            return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
        }
        return false;
    }

    bool RZInput::IsSquarePressed()
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
    bool RZInput::IsKeyPressed(Razix::KeyCode::Key keycode) { return false; }
    bool RZInput::IsKeyReleased(Razix::KeyCode::Key keycode) { return false; }
    bool RZInput::IsKeyHeld(Razix::KeyCode::Key keycode) { return false; }
    bool RZInput::IsMouseButtonPressed(Razix::KeyCode::MouseKey button) { return false; }
    bool RZInput::IsMouseButtonReleased(Razix::KeyCode::MouseKey button) { return false; }
    bool RZInput::IsMouseButtonHeld(Razix::KeyCode::MouseKey button) { return false; }
    std::pair<f32, f32> RZInput::GetMousePosition() { return {0.0f, 0.0f}; }
    f32 RZInput::GetMouseX() { return 0.0f; }
    f32 RZInput::GetMouseY() { return 0.0f; }
    bool RZInput::IsGamepadConnected() { return false; }
    f32 RZInput::GetJoyLeftStickHorizontal() { return 0.0f; }
    f32 RZInput::GetJoyLeftStickVertical() { return 0.0f; }
    f32 RZInput::GetJoyRightStickHorizontal() { return 0.0f; }
    f32 RZInput::GetJoyRightStickVertical() { return 0.0f; }
    f32 RZInput::GetJoyDPadHorizontal() { return 0.0f; }
    f32 RZInput::GetJoyDPadVertical() { return 0.0f; }
    bool RZInput::IsCrossPressed() { return false; }
    bool RZInput::IsCirclePressed() { return false; }
    bool RZInput::IsTrianglePressed() { return false; }
    bool RZInput::IsSquarePressed() { return false; }
#endif

}