#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/OS/KeyCodes.h"

namespace Razix
{
    /// <summary>
    /// Input manager for the Engine to interface with various input devices
	/// such as Keyboard, Mouse, Joystick and other HID devices.
    /// </summary>
    class RAZIX_API Input
    {
    public:
        /// <summary>
        /// Tells whether or not a particular key was pressed or not
        /// </summary>
        /// <param name="keycode"> The code for the key being checked </param>
        /// <returns> True, if the key is pressed </returns>
        inline static bool IsKeyPressed(Razix::KeyCode::Key keycode) { return sInstance->IsKeyPressedImpl(int(keycode)); }

        /// <summary>
        /// Tells whether or not a particular key was released or not
        /// </summary>
        /// <param name="keycode"> The code for the key being checked </param>
        /// <returns> True, if the key is released after an immediate press </returns>
        inline static bool IsKeyReleased(Razix::KeyCode::Key keycode) { return sInstance->IsKeyReleasedImpl(int(keycode)); }

        /// <summary>
        /// Tells whether or not a particular key was being held or not
        /// </summary>
        /// <param name="keycode"> The code for the key being checked </param>
        /// <returns> True, if the key is held </returns>
        inline static bool IsKeyHeld(Razix::KeyCode::Key keycode) { return sInstance->IsIsKeyHeldImpl(int(keycode)); }

        /// <summary>
        /// Tells whether a mouse button was pressed
        /// </summary>
        /// <param name="button"> The mouse button being checked </param>
        /// <returns> True if the mouse button is pressed </returns>
        inline static bool IsMouseButtonPressed(Razix::KeyCode::MouseKey button) { return sInstance->IsMouseButtonPressedImpl(int(button)); }

        /// <summary>
        /// Gets the current position of the mouse
        /// </summary>
        /// <returns> The screen space coordinates of mouse position </returns>
        inline static std::pair<float, float> GetMousePosition() { return sInstance->GetMousePositionImpl(); }

        /// <summary>
        /// The X coordinates of the mouse
        /// </summary>
        /// <returns> Float representing the screen space position of the mouse </returns>
        inline static float GetMouseX() { return sInstance->GetMouseXImpl(); }

		/// <summary>
		/// The Y coordinates of the mouse
		/// </summary>
		/// <returns> Float representing the screen space position of the mouse </returns>
        inline static float GetMouseY() { return sInstance->GetMouseYImpl(); }
    protected:
        /// <summary>
        /// OS/API specific Implementation for the key press
        /// This should be Implementation per OS
        /// </summary>
        virtual bool IsKeyPressedImpl(int keycode) = 0;

		/// <summary>
        /// OS/API specific Implementation for the key release
        /// This should be Implementation per OS
        /// </summary>
        virtual bool IsKeyReleasedImpl(int keycode) = 0;

		/// <summary>
        /// OS/API specific Implementation for the key hold 
        /// This should be Implementation per OS
        /// </summary>
        virtual bool IsIsKeyHeldImpl(int keycode) = 0;

		/// <summary>
		/// OS/API specific Implementation for the mouse button press 
		/// This should be Implementation per OS
		/// </summary>
        virtual bool IsMouseButtonPressedImpl(int button) = 0;

		/// <summary>
        /// OS/API specific Implementation for the mouse position
        /// This should be Implementation per OS
        /// </summary>
        virtual std::pair<float, float> GetMousePositionImpl() = 0;

		/// <summary>
        /// OS/API specific Implementation for the mouse position in X-axis
        /// This should be Implementation per OS
        /// </summary>
        virtual float GetMouseXImpl() = 0;

		/// <summary>
        /// OS/API specific Implementation for the mouse position in Y-axis
        /// This should be Implementation per OS
        /// </summary>
        virtual float GetMouseYImpl() = 0;
    private:
        /// The Global Input variable for the Engine, from which the Input information is retrieved
        static Input* sInstance;
    };
}
