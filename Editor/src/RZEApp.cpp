#pragma once

// clang-format off
#include "rzepch.h"
// clang-format on

#include <Razix.h>

#include <QApplication>
#include <QSettings>
#include <QThread>
#include <QVulkanInstance>

#include "RZEEngineLoop.h"

#include "RZENativeWindow.h"
#include "UI/Widgets/ComponentsUI/RZETransformComponentUI.h"
#include "UI/Widgets/RZECollapsingHeader.h"
#include "UI/Widgets/RZEViewport.h"
#include "UI/Windows/RZEContentBrowserWindow.h"
#include "UI/Windows/RZEInspectorWindow.h"
#include "UI/Windows/RZEMainWindow.h"
#include "UI/Windows/RZEResourceViewer.h"
#include "UI/Windows/RZESceneHierarchyPanel.h"
#include "UI/Windows/RZETitleBar.h"
#include "UI/Windows/RZEVulkanWindow.h"

#include "UI/Windows/RZEFrameGraphEditor.h"

#include "UI/RZEProjectBrowser.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

#include "Styles/StyleData.h"

#include <vulkan/vulkan.h>

// TOOD: Clean this cluster fuck code!!!
static QApplication*                    qrzeditorApp = nullptr;
Razix::Editor::RZEMainWindow*           mainWindow;
Razix::Editor::RZETitleBar*             titlebar;
Razix::Editor::RZEInspectorWindow*      inspectorWidget;
Razix::Editor::RZEViewport*             viewportWidget;
Razix::Editor::RZESceneHierarchyPanel*  sceneHierarchyPanel;
Razix::Editor::RZEContentBrowserWindow* contentBrowserWindow;
Razix::Editor::RZEProjectBrowser*       projectBrowserDialog;
Razix::Editor::RZEMaterialEditor*       materialEditor;
Razix::Editor::RZEResourceViewer*       resourceViewer;

Razix::Editor::RZEFrameGraphEditor* framegraphEditor;

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

        // Add any QT UI after the Application start up
        QMetaObject::invokeMethod(qrzeditorApp, [] {
            // We defer the content browser until a later stage for the engine to mount VFS
            contentBrowserWindow = new Razix::Editor::RZEContentBrowserWindow;
            mainWindow->addDockableWidget(contentBrowserWindow, "Content Browser");

            // We defer resource view UI until after the Post-Graphics initialization and API has been initialized
            resourceViewer = new Razix::Editor::RZEResourceViewer;
            mainWindow->addDockableWidget(resourceViewer, "Resource Viewer");
        });

        VkSurfaceKHR                surface = QVulkanInstance::surfaceForWindow(vulkanWindow);
        Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());
        context->CreateSurface(&surface);
        context->SetupDeviceAndSC();

        vulkanWindow->getRZNativeWindow()->setWidth(vulkanWindow->width());
        vulkanWindow->getRZNativeWindow()->setHeight(vulkanWindow->height());

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
            mainWindow->restoreLayout();
            mainWindow->show();
            if (titlebar)
                titlebar->show();
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

// TODO: move the ifdef for platforms defined in EntryPoint to here and to Sandbox
HINSTANCE razixDll;

void LoadEngineDLL(int argc, char** argv)
{
    razixDll = LoadLibrary(L"Razix.dll");
    EngineMain(argc, argv);
    didEngineClose = true;
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

    mainWindow = new Razix::Editor::RZEMainWindow;
#ifdef ENABLE_CUSTOM_TITLE_BAR
    titlebar = new Razix::Editor::RZETitleBar(mainWindow);
#endif
    mainWindow->setWindowTitle("Razix Engine Editor");
    mainWindow->resize(1280, 720);
    mainWindow->setWindowState(Qt::WindowMaximized);
    //mainWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    mainWindow->setProjectPathDir(projectBrowserDialog->getProjectPath());

    // Register the Qt Consoler Logger Sinks
    Razix::Debug::RZLog::RegisterCoreLoggerSink(mainWindow->getConsolerLoggerSink());

    // Init the Windows
    materialEditor = new Razix::Editor::RZEMaterialEditor;
    //mainWindow->getToolWindowManager()->addToolWindow(materialEditor, ToolWindowManager::AreaReference(ToolWindowManager::RightWindowSide));
    mainWindow->addDockableWidget(materialEditor, "Material Editor");

    sceneHierarchyPanel = new Razix::Editor::RZESceneHierarchyPanel(mainWindow);

    inspectorWidget = new Razix::Editor::RZEInspectorWindow(sceneHierarchyPanel);
    viewportWidget  = new Razix::Editor::RZEViewport(sceneHierarchyPanel);
    viewportWidget->resize(1280, 720);
    viewportWidget->setWindowTitle("Viewport");
    inspectorWidget->setWindowTitle("Inspector");
    inspectorWidget->setAutoFillBackground(true);
    QIcon razixIcon(":/rzeditor/RazixLogo64.png");
    inspectorWidget->setWindowIcon(razixIcon);
    viewportWidget->setWindowIcon(razixIcon);
    viewportWidget->show();

    //mainWindow->getToolWindowManager()->addToolWindow(inspectorWidget, ToolWindowManager::AreaReference(ToolWindowManager::LastUsedArea));
    mainWindow->addDockableWidget(inspectorWidget, "Inspector");

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
    mainWindow->addDockableWidget(sceneHierarchyPanel, "Scene Hierarchy");

    // In order for event filter to work this is fookin important
    qrzeditorApp->installEventFilter(viewportWidget->getVulkanWindow());

    //mainWindow->getToolWindowManager()->addToolWindow(viewportWidget, ToolWindowManager::AreaReference(ToolWindowManager::AddTo, mainWindow->getToolWindowManager()->areaOf(inspectorWidget)));
    // FIXME: Add this to right of some other window
    mainWindow->addDockableWidget(viewportWidget, "Viewport");

    viewportWidget->resize(1280, 720);

#if 1
    framegraphEditor = new Razix::Editor::RZEFrameGraphEditor;
    mainWindow->addDockableWidget(framegraphEditor, "Frame Graph Editor");
#endif

    // Load the engine DLL and Ignite it on a separate thread
    QThread* qengineThread = new QThread;

    //std::thread engineThread(LoadEngineDLL, argc, argv);
    //engineThread.detach();
    Razix::Editor::RZEEngineLoop* engineLoop = new Razix::Editor::RZEEngineLoop(argc, argv);
    engineLoop->moveToThread(qengineThread);
    viewportWidget->moveToThread(qengineThread);
    viewportWidget->getVulkanWindow()->moveToThread(qengineThread);
    QObject::connect(qengineThread, &QThread::started, engineLoop, [&]() {
        EngineMain(argc, argv);

        // Do the work Here
        while (Razix::RZApplication::Get().RenderFrame()) {}

        Razix::RZApplication::Get().Quit();
        Razix::RZApplication::Get().SaveApp();

        EngineExit();
        didEngineClose = true;
    });

    //engineLoop->launch();
    qengineThread->start();

    int r = qrzeditorApp->exec();

    qengineThread->quit();
    qengineThread->wait();
    // Wait for engine to completely close
    while (!didEngineClose) {}
    return r;
}