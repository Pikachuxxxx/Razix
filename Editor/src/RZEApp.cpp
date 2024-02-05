#pragma once

// clang-format off
#include "rzepch.h"
// clang-format on

#include <Razix.h>
#include <Razix/Core/RazixVersion.h>
#include <Razix/Platform/API/Vulkan/VKContext.h>

#include <vulkan/vulkan.h>

#include <QApplication>
#include <QSettings>
#include <QStyleFactory>
#include <QThread>
#include <QVulkanInstance>

#include "RZEEngineLoop.h"
#include "RZENativeWindow.h"
#include "UI/Widgets/ComponentsUI/RZETransformComponentUI.h"
#include "UI/Widgets/RZECollapsingHeader.h"
#include "UI/Widgets/RZEViewport.h"
#include "UI/Windows/RZEApplicationMainDockWindow.h"
#include "UI/Windows/RZEContentBrowserWindow.h"
#include "UI/Windows/RZEFrameGraphEditor.h"
#include "UI/Windows/RZEInspectorWindow.h"
#include "UI/Windows/RZEResourceViewer.h"
#include "UI/Windows/RZESceneHierarchyPanel.h"
#include "UI/Windows/RZETitleBar.h"
#include "UI/Windows/RZEVulkanWindow.h"

#include "UI/RZEProjectBrowser.h"

#include "Styles/StyleData.h"

// TOOD: Clean this cluster fuck code!!!
static QApplication*                    qrzeditorApp         = nullptr;
Razix::Editor::RZEAppMainWindow*        mainWindow           = nullptr;    //Razix::Editor::RZEApplicationMainDockWindowCentralWidget*
Razix::Editor::RZETitleBar*             titlebar             = nullptr;
Razix::Editor::RZEInspectorWindow*      inspectorWidget      = nullptr;
Razix::Editor::RZEViewport*             viewportWidget       = nullptr;
Razix::Editor::RZESceneHierarchyPanel*  sceneHierarchyPanel  = nullptr;
Razix::Editor::RZEContentBrowserWindow* contentBrowserWindow = nullptr;
Razix::Editor::RZEProjectBrowser*       projectBrowserDialog = nullptr;
Razix::Editor::RZEMaterialEditor*       materialEditor       = nullptr;
Razix::Editor::RZEResourceViewer*       resourceViewer       = nullptr;
Razix::Editor::RZEFrameGraphEditor*     framegraphEditor     = nullptr;

bool didEngineClose = false;

using namespace Razix;

class RazixEditorApp : public Razix::RZApplication
{
public:
    // TODO: In future we will pass multiple native window handles (for multiple viewports, debug rendering, content viewers etc) for now only a single viewport is sufficient
    RazixEditorApp(std::string projectFilePath, std::string projectName)
        : RZApplication(projectFilePath, projectName)
    {
        // Show the Editor Application after the engine static initialization is complete

        Razix::RZApplication::Get().setAppType(Razix::AppType::EDITOR);

        auto vulkanWindow = viewportWidget->getVulkanWindow();

        // get the hwnd handle
        HWND hWnd = (HWND) vulkanWindow->winId();

        vulkanWindow->InitRZWindow();

        Razix::Editor::RZENativeWindow::Construct();
        Razix::RZApplication::setViewportWindow(vulkanWindow->getRZNativeWindow() /*Razix::RZWindow::Create(&hWnd, properties)*/ /* TODO: Create the window by taking it from vulkanWindow and binding the event callbacks so update the properties properly */);
        Razix::RZApplication::Get().setViewportHWND(hWnd);

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Graphics::RZGraphicsContext::GetContext()->Init();
        //-------------------------------------------------------------------------------------

        vulkanWindow->Init();

        vulkanWindow->setTitle("Vulkan Window");

        Razix::RZApplication::Get().Init();

        VkSurfaceKHR                surface = QVulkanInstance::surfaceForWindow(vulkanWindow);
        Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());
        context->CreateSurface(&surface);
        context->SetupDeviceAndSC();

        vulkanWindow->getRZNativeWindow()->setWidth(vulkanWindow->width());
        vulkanWindow->getRZNativeWindow()->setHeight(vulkanWindow->height());

        // Add any QT UI after the Application start up
        QMetaObject::invokeMethod(qrzeditorApp, [] {
            // We defer the content browser until a later stage for the engine to mount VFS
            contentBrowserWindow = new Razix::Editor::RZEContentBrowserWindow;
            mainWindow->getCentralWidget()->addDockableWidget(contentBrowserWindow, "Content Browser");

            // We defer resource view UI until after the Post-Graphics initialization and API has been initialized
            resourceViewer = new Razix::Editor::RZEResourceViewer;
            mainWindow->getCentralWidget()->addDockableWidget(resourceViewer, "Resource Viewer");
        });

        std::lock_guard<std::mutex> lk(RZApplication::m);
        RZApplication::ready_for_execution = true;
        RAZIX_INFO("Triggering worker thread to resume execution ::::");
        RZApplication::halt_execution.notify_one();
    }

private:
    void RAZIX_CALL OnStart() override
    {
        // Force Restore the Layout here
        QMetaObject::invokeMethod(qrzeditorApp, [] {
            mainWindow->getCentralWidget()->restoreLayout();
            mainWindow->show();
        });

        RZSceneManager::Get().loadScene(0);
        RZScene* activeScene = RZSceneManager::Get().getCurrentScene();

        // This won't actually work though! cause by the time we reach here we will have a default scene
        if (!activeScene) {
            RAZIX_TRACE("Creatng new scene...");
            Razix::RZScene* editormodelLightScene = new Razix::RZScene("Editor_Scene_1");
            RZSceneManager::Get().enqueScene(editormodelLightScene);
            RZSceneManager::Get().loadScene();
            activeScene = RZSceneManager::Get().getCurrentScene();
        }

        // Add entities to the scene programatically for the first time
        // Camera Entity
        auto cameras = activeScene->GetComponentsOfType<CameraComponent>();
        if (!cameras.size()) {
            RZEntity camera = activeScene->createEntity("Camera");
            camera.AddComponent<CameraComponent>();
            if (camera.HasComponent<CameraComponent>()) {
                CameraComponent& cc = camera.GetComponent<CameraComponent>();
                cc.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
            }
        }

        auto scripts = activeScene->GetComponentsOfType<LuaScriptComponent>();
        if (!scripts.size()) {
            RZEntity scriptableEntity = activeScene->createEntity("hello_lua");
            scriptableEntity.AddComponent<LuaScriptComponent>();
            if (scriptableEntity.HasComponent<LuaScriptComponent>()) {
                LuaScriptComponent& lsc = scriptableEntity.GetComponent<LuaScriptComponent>();
                lsc.loadScript("//Scripts/hello_razix.lua");
            }

            RZEntity imguiEntity = activeScene->createEntity("imgui_lua");
            imguiEntity.AddComponent<LuaScriptComponent>();
            if (imguiEntity.HasComponent<LuaScriptComponent>()) {
                LuaScriptComponent& lsc = imguiEntity.GetComponent<LuaScriptComponent>();
                lsc.loadScript("//Scripts/imgui_test.lua");
            }
        }

        // Add a directional light for test
        auto lightEnitties = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::LightComponent>();
        if (!lightEnitties.size()) {
            auto directionalLightEntity = RZSceneManager::Get().getCurrentScene()->createEntity("Directional Light");
            directionalLightEntity.AddComponent<Razix::LightComponent>();
        }

        //auto meshEnitties = activeScene->GetComponentsOfType<MeshRendererComponent>();
        //if (!meshEnitties.size()) {
        //    auto planeMesh = activeScene->createEntity("CubeMesh");
        //    planeMesh.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Cube);
        //}

        QMetaObject::invokeMethod(qrzeditorApp, [] {
            sceneHierarchyPanel->populateHierarchy();
        });
    }

    void RAZIX_CALL OnResize(uint32_t width, uint32_t height) override
    {
        std::lock_guard<std::mutex> lk(RZApplication::m);
        RZApplication::ready_for_execution = true;
        RAZIX_INFO("Triggering worker thread to resume execution ::::");
        RZApplication::halt_execution.notify_one();

        RAZIX_TRACE("Window Resize override Editor application! | W : {0}, H : {1}", width, height);
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Editor Application");
    return new RazixEditorApp(projectBrowserDialog->getProjectPath(), projectBrowserDialog->getProjectName());
}

int main(int argc, char** argv)
{
    // Initialize the QT Editor Application
    qrzeditorApp = new QApplication(argc, argv);
    qrzeditorApp->setOrganizationName("Razix Engine");
    qrzeditorApp->setApplicationName("Razix Editor");

    // Store the app settings as a .ini file
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // RenderDoc style from source file
    //QStyle* style = StyleData::availStyles[1].creator();
    //QApplication::setStyle(style);

    // Set Fusion style for editor
    qrzeditorApp->setStyle(QStyleFactory::create("Fusion"));
    // modify palette to dark
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(15, 15, 15));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Base, QColor(24, 24, 24));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    qrzeditorApp->setPalette(darkPalette);

    // UE5 like style from qss
    QFile file(":/rzeditor/styles/ue.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qrzeditorApp->setStyleSheet(styleSheet);

    qrzeditorApp->setWindowIcon(QIcon(":/rzeditor/RazixLogo64.png"));

    // First we kickoff the project browser Dialog before anything starts
    projectBrowserDialog = new Razix::Editor::RZEProjectBrowser;
    if (!projectBrowserDialog->exec())
        return 69;

    printf("Project Name : %s \n", projectBrowserDialog->getProjectName().c_str());
    printf("Project Path : %s \n", projectBrowserDialog->getProjectPath().c_str());

    mainWindow = new Editor::RZEAppMainWindow;    // new Razix::Editor::RZEApplicationMainDockWindowCentralWidget;
#ifdef ENABLE_CUSTOM_TITLE_BAR
    titlebar = new Razix::Editor::RZETitleBar(mainWindow);
#endif
    mainWindow->setWindowTitle("Razix Engine Editor");
    mainWindow->resize(1280, 720);
    mainWindow->setWindowState(Qt::WindowMaximized);
    //mainWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    mainWindow->getCentralWidget()->setProjectPathDir(projectBrowserDialog->getProjectPath());

    mainWindow->setStyleSheet(R"(
    QMainWindow::separator {
      width: 10px;
      height: 10px;
      background-color: #F00;
    }
  )");

    // Set Title Bar project and engine version
    mainWindow->getTitleBar()->setProjectName(projectBrowserDialog->getProjectName().c_str());
    mainWindow->getTitleBar()->setBuildVersion("Build : V." + Razix::RazixVersion.getVersionString());

    // Register the Qt Consoler Logger Sinks
    Razix::Debug::RZLog::RegisterCoreLoggerSink(mainWindow->getCentralWidget()->getConsolerLoggerSink());

    // Init the Windows
    materialEditor = new Razix::Editor::RZEMaterialEditor;
    //mainWindow->getToolWindowManager()->addToolWindow(materialEditor, ToolWindowManager::AreaReference(ToolWindowManager::RightWindowSide));
    mainWindow->getCentralWidget()->addDockableWidget(materialEditor, "Material Editor");

    sceneHierarchyPanel = new Razix::Editor::RZESceneHierarchyPanel(mainWindow->getCentralWidget());

    inspectorWidget = new Razix::Editor::RZEInspectorWindow(sceneHierarchyPanel);
    viewportWidget  = new Razix::Editor::RZEViewport(sceneHierarchyPanel);
    viewportWidget->resize(1280, 720);
    viewportWidget->setWindowTitle("Viewport");
    inspectorWidget->setWindowTitle("Inspector");
    inspectorWidget->setAutoFillBackground(true);
    QIcon razixIcon(":/rzeditor/RazixLogo64.png");
    inspectorWidget->setWindowIcon(razixIcon);
    viewportWidget->setWindowIcon(razixIcon);
    /**
     * THIS IS ABSOLUTELY NECESSARY FOR RENDERING TO WORK!
     */
    //-----------------------
    viewportWidget->show();
    //-----------------------

    //mainWindow->getToolWindowManager()->addToolWindow(inspectorWidget, ToolWindowManager::AreaReference(ToolWindowManager::LastUsedArea));
    mainWindow->getCentralWidget()->addDockableWidget(inspectorWidget, "Inspector");

    // Connect the Signal from Inspector window to a slot in Material editor to set the material if a mesh is selected
    QObject::connect(inspectorWidget, &Razix::Editor::RZEInspectorWindow::OnMeshMaterialSelected, materialEditor, &Razix::Editor::RZEMaterialEditor::OnSetEditingMaterial);

    //mainWindow->getToolWindowManager()->addToolWindow(viewportWidget, ToolWindowManager::AreaReference(ToolWindowManager::LastUsedArea /*ToolWindowManager::AddTo, mainWindow->getToolWindowManager()->areaOf(inspectorWidget))*/));

    //vulkanWindow = new Razix::Editor::RZEVulkanWindow();
    //vulkanWindow->show();
    ////auto vulkanWindowWidget = QWidget::createWindowContainer(vulkanWindow);

    //vulkanWindow->setWindowIcon(razixIcon);
    //vulkanWindow->setWindowTitle("Viewport Window (VK)");
    // vulkanWindowWidget->resize(1280, 720);
    // vulkanWindowWidget->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    // Scene Hierarchy
    //mainWindow->getToolWindowManager()->addToolWindow(sceneHierarchyPanel, ToolWindowManager::AreaReference(ToolWindowManager::LeftOf, mainWindow->getToolWindowManager()->areaOf(inspectorWidget), 0.2f));
    mainWindow->getCentralWidget()->addDockableWidget(sceneHierarchyPanel, "Scene Hierarchy");

    // In order for event filter to work this is fookin important
    qrzeditorApp->installEventFilter(viewportWidget->getVulkanWindow());

    //mainWindow->getToolWindowManager()->addToolWindow(viewportWidget, ToolWindowManager::AreaReference(ToolWindowManager::AddTo, mainWindow->getToolWindowManager()->areaOf(inspectorWidget)));
    // FIXME: Add this to right of some other window
    mainWindow->getCentralWidget()->addDockableWidget(viewportWidget, "Viewport");

    viewportWidget->resize(1280, 720);

#if 1
    framegraphEditor = new Razix::Editor::RZEFrameGraphEditor;
    mainWindow->getCentralWidget()->addDockableWidget(framegraphEditor, "Frame Graph Editor");
#endif

    // Load the engine DLL and Ignite it on a separate thread, using a worker Object to execute some work, here RZEEngineLoop is with worker that will run the engine code
    Razix::Editor::RZEEngineLoop* engineLoop    = new Razix::Editor::RZEEngineLoop(argc, argv);
    QThread*                      qengineThread = new QThread();
    // Move some objects to be accessed on the threads
    engineLoop->moveToThread(qengineThread);
    viewportWidget->moveToThread(qengineThread);
    viewportWidget->getVulkanWindow()->moveToThread(qengineThread);
    // Run the thread loop
    QObject::connect(qengineThread, &QThread::started, engineLoop, [&]() {
        // Call engine main
        EngineMain(argc, argv);

        // Engine Main rendering Loop
        while (Razix::RZApplication::Get().RenderFrame()) {}

        // Quit and Save on exit
        Razix::RZApplication::Get().Quit();
        Razix::RZApplication::Get().SaveApp();

        // Engine Exit
        EngineExit();
        didEngineClose = true;
    });

    //--------------------------------------------------
    // Start the Engine on a separate thread than QT app
    qengineThread->start();
    //--------------------------------------------------

    // Execute the Application
    int r = qrzeditorApp->exec();

    // Now quit and wait until we get out of qthread and sync with main editor UI thread
    qengineThread->quit();
    qengineThread->wait();

    // Wait for engine to completely close on this main QT UI thread
    while (!didEngineClose) {}

    // delete the pointer
    // Delete all the UI objects
    // TODO: This is temporary, use a better manager class/system, no ptrs allowed in anyway whatsoever (smart too)
    {
        delete qengineThread;
        delete engineLoop;
        delete qrzeditorApp;
        delete mainWindow;
        delete titlebar;
        delete inspectorWidget;
        delete viewportWidget;
        delete sceneHierarchyPanel;
        delete contentBrowserWindow;
        delete projectBrowserDialog;
        delete materialEditor;
        delete resourceViewer;
        delete framegraphEditor;
    }

    return r;
}