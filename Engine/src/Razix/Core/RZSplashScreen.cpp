// clang-format off
#include "rzxpch.h"
// clang-format on

#ifdef RAZIX_PLATFORM_WINDOWS
    #include "RZSplashScreen.h"
    #include "Razix/Core/RazixVersion.h"

    #include "Razix/Core/RZEngine.h"

    #include <sstream>
    #include <thread>

namespace Razix {
    // Window Class Construction for the static singleton variable
    RZSplashScreen::WindowClass RZSplashScreen::WindowClass::wndClass;

    RZSplashScreen::RZSplashScreen()
    {
        // Select the splash image based on the release stage
        if (RazixVersion.getReleaseStage() == Version::Stage::Development)
            m_ImagePath = RZEngine::Get().getEngineInstallationDir() + std::string("/Engine/content/Splash/RazixSplashScreenDev.bmp");
        else if (RazixVersion.getReleaseStage() == Version::Stage::Alpha)
            m_ImagePath = RZEngine::Get().getEngineInstallationDir() + std::string("/Engine/content/Splash/RazixSplashScreenAlpha2.bmp");

        // Create Window Instance & get hWnd
        hWnd = CreateWindow(
            WindowClass::getName(), "Splash Screen", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0 - (480 / 2), 0 - (320 / 2), 480, 320, nullptr, nullptr, WindowClass::getInstance(), this);

        // Get the total dim of the screen
        HMONITOR    monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info;
        info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(monitor, &info);
        int monitor_width  = info.rcMonitor.right - info.rcMonitor.left;
        int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

        SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);    // remove all window styles, check MSDN for details
        SetWindowPos(hWnd, 0, monitor_width / 2 - (480 / 2), monitor_height / 2 - (320 / 2), 480, 320, SWP_FRAMECHANGED);

        // newly created windows start off as hidden
        ShowWindow(hWnd, SW_SHOW);
    }

    RZSplashScreen::~RZSplashScreen()
    {
        DestroyWindow(hWnd);
    }

    void RZSplashScreen::init()
    {
        //std::thread splashThread(this->ProcessMessages());
        ProcessMessages();
    }

    void RZSplashScreen::destroy()
    {
        DestroyWindow(hWnd);
    }

    void RZSplashScreen::setVersionString(const std::string& text)
    {
        m_VersionString = text;
        SendMessage(hWnd, UPDATE_VERSION_LABEL, NULL, NULL);
    }

    void RZSplashScreen::setLogString(const std::string& text)
    {
        m_LogString = text;
        SendMessage(hWnd, UPDATE_VERSION_LABEL, NULL, NULL);
    }

    std::optional<int> RZSplashScreen::ProcessMessages()
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return (int) msg.wParam;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return {};
    }

    LRESULT CALLBACK RZSplashScreen::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
        if (msg == WM_NCCREATE) {
            // extract ptr to window class from creation data
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            RZSplashScreen* const      pWnd    = static_cast<RZSplashScreen*>(pCreate->lpCreateParams);
            // set WinAPI-managed user data to store ptr to window instance
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            // set message proc to normal (non-setup) handler now that setup is finished
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&RZSplashScreen::HandleMsgThunk));
            // forward message to window instance handler
            return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
        }
        // if we get a message before the WM_NCCREATE message, handle with default handler
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK RZSplashScreen::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // retrieve ptr to window instance
        RZSplashScreen* const pWnd = reinterpret_cast<RZSplashScreen*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        // forward message to window instance handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }

    /* Message callback Handler */
    LRESULT RZSplashScreen::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Maps the message map ID to map it to a string
        //OutputDebugString(mm(msg, wParam, lParam).c_str());

        switch (msg) {
            case WM_CLOSE:
                PostQuitMessage(69);
                break;
            case WM_CREATE: {
                // Add background image
                m_SplashImage     = (HBITMAP) LoadImage(NULL, m_ImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                m_SplashImageView = CreateWindowEx(NULL, "STATIC", NULL, SS_BITMAP | WS_VISIBLE | WS_CHILD, 0, 0, 480, 320, hWnd, 0, GetModuleHandle(NULL), NULL);
                SendMessage(m_SplashImageView, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) m_SplashImage);
                break;
            }
            case WM_PAINT: {
                m_VersionLabel = CreateWindowEx(NULL, "STATIC", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD, 220, 220, 240, 20, hWnd, 0, GetModuleHandle(NULL), NULL);
                m_LogLabel     = CreateWindowEx(NULL, "STATIC", NULL, SS_LEFT | WS_VISIBLE | WS_CHILD, 20, 280, 240, 20, hWnd, 0, GetModuleHandle(NULL), NULL);
                SetWindowTextA(m_VersionLabel, "Version : 0.15.0 [DEVELOPMENT]");
                SetWindowTextA(m_LogLabel, "Initializing Logging system...");
                break;
            }
            case WM_CTLCOLORSTATIC: {
                HDC hdc;
                hdc = (HDC) wParam;
                SetTextColor(hdc, RGB(255, 255, 255));
                SetTextAlign(hdc, TA_LEFT);
                SetBkMode(hdc, TRANSPARENT);

                return (LRESULT) GetStockObject(NULL_BRUSH);
            }
            case WM_NCHITTEST: {
                LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
                if (hit == HTCLIENT) hit = HTCAPTION;
                return hit;
                break;
                ;
            }
            // Custom Messages
            case UPDATE_VERSION_LABEL: {
                SetWindowTextA(m_VersionLabel, m_VersionString.c_str());
                RedrawWindow(m_VersionLabel, NULL, NULL, RDW_ERASE);
                ShowWindow(m_VersionLabel, SW_HIDE);
                ShowWindow(m_VersionLabel, SW_SHOW);

                SetWindowTextA(m_LogLabel, m_LogString.c_str());
                RedrawWindow(m_LogLabel, NULL, NULL, RDW_ERASE);
                ShowWindow(m_LogLabel, SW_HIDE);
                ShowWindow(m_LogLabel, SW_SHOW);

                SendMessage(m_SplashImageView, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) m_SplashImage);
                UpdateWindow(m_SplashImageView);
                UpdateWindow(hWnd);
                break;
            }
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    /*************************************************************************
     *						WindowsWindow::Window Class                      *
    *************************************************************************/

    RZSplashScreen::WindowClass::WindowClass() noexcept
        : hInst(GetModuleHandle(nullptr))
    {
        // Register the Window Class using a configuration struct
        WNDCLASSEX windowClass    = {0};
        windowClass.cbSize        = sizeof(windowClass);
        windowClass.style         = CS_OWNDC;          // Creates multiple windows with it's own device context (CS = class style, OWN DC =  own device context)
        windowClass.lpfnWndProc   = HandleMsgSetup;    // The window procedure to handle the messages
        windowClass.cbClsExtra    = 0;                 // No extra used defines data for the custom data for current window class registered
        windowClass.cbWndExtra    = 0;                 // Same as above but it's the data for every instance of this class
        windowClass.hInstance     = getInstance();     // The instance handler for the current window
        windowClass.hIcon         = nullptr;           // static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));				    // Icon
        windowClass.hCursor       = nullptr;           // Using the default cursor
        windowClass.hbrBackground = nullptr;           // We take care of this through DirectX, We don't specify how to clear the background we leave it empty to be taken care by DirectX
        windowClass.lpszMenuName  = nullptr;           // Not using any menus
        windowClass.lpszClassName = getName();         // The name to identify this class handle
        windowClass.hIconSm       = nullptr;           // static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));;
        RegisterClassEx(&windowClass);
    }

    RZSplashScreen::WindowClass::~WindowClass()
    {
        UnregisterClass(wndClassName, getInstance());
    }

    cstr RZSplashScreen::WindowClass::getName() noexcept
    {
        return wndClassName;
    }

    HINSTANCE RZSplashScreen::WindowClass::getInstance() noexcept
    {
        return wndClass.hInst;
    }
}    // namespace Razix
#endif    // RAZIX_PLATFORM_WINDOWS