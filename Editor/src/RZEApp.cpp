#pragma once

#include <Razix.h>

#include <QApplication>

#include "UI/Windows/RZEMainWindow.h"

static  QApplication* qrzeditorApp = nullptr;

class RazixEditorApp : public Razix::RZApplication
{
public:
    Razix::Editor::RZEMainWindow mainWindow;

public:

    // TODO: In future we will pass multiple native window handles (for multiple viewports, debug rendering, content viewers etc) for now only a single viewport is sufficient
    RazixEditorApp(void* viewportNativeWindowHandle)
        : RZApplication("/Sandbox/", "Razix Editor")
    {
        // Show the Editor Application after the engine static initialization is complete

        mainWindow.resize(800, 600);
        mainWindow.show();
    }

private:

    void RAZIX_CALL OnUpdate(const Razix::RZTimestep& dt) override
    {
        // To avoid the blocking QApplication::exec() we use this and we nicely have a proper event update loop
        // But this makes the engine and editor to run the from the same main thread!!! (ofc once the engine become multi-threaded this won't affect much as only a few systems would be on the main thread)
        // This also simplifies engine-editor communication as it resolves
        qrzeditorApp->processEvents();
    }

    void RAZIX_CALL OnResize(uint32_t width, uint32_t height) override
    {
        
    }

    void RAZIX_CALL OnQuit() override
    {
        
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Editor Application");

    // Create the QApplication here and run it on a separate thread + pass the necessary native window handles to the Razix Application
    qrzeditorApp = new QApplication(argc, argv);

    return new RazixEditorApp(nullptr);
}