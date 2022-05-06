#pragma once

#ifdef RAZIX_PLATFORM_WINDOWS

    #include <iostream>
    #include <memory>
    #include <optional>
    #include <stdint.h>

    #include <Windows.h>

    #include "Razix/Core/RZCore.h"
    #include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    #define UPDATE_VERSION_LABEL (WM_APP + 20)    // We choose some arbitrary ID to register our custom event such so as to update the splash labels

    // TODO: Fix the style guide enforcements for this class and comment everything properly
    /* Creates a Splash Screen while the Engine systems are loading */
    class RAZIX_API RZSplashScreen : public RZSingleton<RZSplashScreen>
    {
    private:
        /* singleton manages registration / cleanup of window class */
        class WindowClass
        {
        public:
            /* Gets the windows message */
            static const char* getName() noexcept;
            /* Gets the Win32 windows instance */
            static HINSTANCE getInstance() noexcept;

        private:
            /* Abstracts the Windows window registration */
            WindowClass() noexcept;
            ~WindowClass();

            // Singleton instance
            static WindowClass           wndClass;
            static constexpr const char* wndClassName = "Razix Engine Splash Screen";
            HINSTANCE                    hInst;
        };

    public:
        RZSplashScreen();
        ~RZSplashScreen();

        void init();
        void destroy();

        /* Set version string for the splash screen */
        void setVersionString(const std::string& text);
        /* Set the log string for the splash screen */
        void setLogString(const std::string& text);

        /* Process the windows messages */
        static std::optional<int> ProcessMessages();

    private:
        int         width;
        int         height;
        int32_t     xPos, yPos;
        HWND        hWnd;
        std::string m_VersionString;
        std::string m_LogString;
        std::string m_ImagePath;
        HBITMAP     m_SplashImage;
        HWND        m_SplashImageView;
        HWND        m_VersionLabel;
        HWND        m_LogLabel;

    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT                 HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    };
}    // namespace Razix
#endif    // RAZIX_PLATFORM_WINDOWS
