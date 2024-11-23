// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZENativeWindow.h"

#define RAZIX_EDITOR_BIND_CB_EVENT_FN(x) std::bind(&Razix::Editor::RZENativeWindow::x, this, std::placeholders::_1)

namespace Razix {
    namespace Editor {
        RZENativeWindow::RZENativeWindow(void* windowHandle, const WindowProperties& properties)
        {
            m_Data.Title  = properties.Title;
            m_Data.Width  = properties.Width;
            m_Data.Height = properties.Height;

            m_NativeHandle = windowHandle;

            SetEventCallback(RAZIX_EDITOR_BIND_CB_EVENT_FN(RZENativeWindow::OnEventDefaultBind));
        }

        void RZENativeWindow::OnWindowUpdate()
        {
        }

        void RZENativeWindow::Destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        unsigned int RZENativeWindow::getWidth() const
        {
            return m_Data.Width;
        }

        unsigned int RZENativeWindow::getHeight() const
        {
            return m_Data.Height;
        }

        void RZENativeWindow::SetEventCallback(const EventCallbackFn& callback)
        {
            EventCallback = callback;
        }

        void RZENativeWindow::SetVSync(bool enabled)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        bool RZENativeWindow::IsVSync() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZENativeWindow::SetWindowIcon()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void* RZENativeWindow::GetNativeWindow() const
        {
            return (HWND*) m_NativeHandle;
        }

        RZWindow* RZENativeWindow::NativeConstructionFunc(void* windowHandle, const WindowProperties& properties)
        {
            return new RZENativeWindow(windowHandle, properties);
        }

        void RZENativeWindow::Construct()
        {
            ConstructionFuncNative = NativeConstructionFunc;
        }

    }    // namespace Editor
}    // namespace Razix