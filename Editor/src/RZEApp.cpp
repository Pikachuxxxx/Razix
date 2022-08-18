#pragma once

#include <Razix.h>

#include <QApplication>
#include <QVulkanInstance>

#include "RZENativeWindow.h"
#include "UI/Widgets/ComponentsUI/RZETransformComponentUI.h"
#include "UI/Widgets/RZECollapsingHeader.h"
#include "UI/Widgets/RZEViewport.h"
#include "UI/Windows/RZEContentBrowserWindow.h"
#include "UI/Windows/RZEInspectorWindow.h"
#include "UI/Windows/RZEMainWindow.h"
#include "UI/Windows/RZESceneHierarchyPanel.h"
#include "UI/Windows/RZEVulkanWindow.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

#include "Styles/StyleData.h"

#include <vulkan/vulkan.h>

static QApplication*                    qrzeditorApp = nullptr;
Razix::Editor::RZEMainWindow*           mainWindow;
Razix::Editor::RZEVulkanWindow*         vulkanWindow;
Razix::Editor::RZEInspectorWindow*      inspectorWidget;
Razix::Editor::RZEViewport*             viewportWidget;
Razix::Editor::RZESceneHierarchyPanel*  sceneHierarchyPanel;
Razix::Editor::RZEContentBrowserWindow* contentBrowserWindow;

using namespace Razix;

class RazixEditorApp : public Razix::RZApplication
{
public:
public:
    // TODO: In future we will pass multiple native window handles (for multiple viewports, debug rendering, content viewers etc) for now only a single viewport is sufficient
    RazixEditorApp()
        : RZApplication("/Sandbox/", "Razix Editor")
    {
        // Show the Editor Application after the engine static initialization is complete

        Razix::RZApplication::Get().setAppType(Razix::AppType::EDITOR);

        // get the hwnd handle
        HWND hWnd = (HWND) vulkanWindow->winId();

        vulkanWindow->InitRZWindow();

        Razix::Editor::RZENativeWindow::Construct();
        Razix::RZApplication::setViewportWindow(vulkanWindow->getRZNativeWindow() /*Razix::RZWindow::Create(&hWnd, properties)*/ /* TODO: Create the window by taking it from vulkanWindow and binding the event callbacks so update the properties properly */);
        Razix::RZApplication::Get().setViewportHWND(hWnd);

        auto h = vulkanWindow->height();

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

        QMetaObject::invokeMethod(qrzeditorApp, [] {
            contentBrowserWindow = new Razix::Editor::RZEContentBrowserWindow;
            mainWindow->getToolWindowManager()->addToolWindow(contentBrowserWindow, ToolWindowManager::AreaReference(ToolWindowManager::BottomOf, mainWindow->getToolWindowManager()->areaOf(inspectorWidget)));
        });

        VkSurfaceKHR                surface = QVulkanInstance::surfaceForWindow(vulkanWindow);
        Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());
        context->CreateSurface(&surface);
        context->SetupDeviceAndSC();

        vulkanWindow->getRZNativeWindow()->setWidth(vulkanWindow->width());
        vulkanWindow->getRZNativeWindow()->setHeight(vulkanWindow->height());

        // Since it will be locked/halted for initial resize that is called when the QT window is created we manually trigger it again to resume execution
        std::lock_guard<std::mutex> lk(RZApplication::m);
        RZApplication::ready_for_execution = true;
        std::cout << "Triggering worker thread to resume execution :::: " << std::endl;
        RZApplication::halt_execution.notify_one();
    }

private:
    void RAZIX_CALL OnStart() override
    {
        Razix::RZEngine::Get().getSceneManager().loadScene(0);
        RZScene* activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();

        if (!activeScene) {
            RAZIX_TRACE("Creatng new scene...");
            Razix::RZScene* editormodelLightScene = new Razix::RZScene("Editor_Scene_1");
            Razix::RZEngine::Get().getSceneManager().enqueScene(editormodelLightScene);
            Razix::RZEngine::Get().getSceneManager().loadScene();
            activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
        }

        // Add entities to the scene programatically for the first time
        // Camera Entity
        auto& cameras = activeScene->GetComponentsOfType<CameraComponent>();
        if (!cameras.size()) {
            RZEntity& camera = activeScene->createEntity("Camera");
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

        // Add some model entities
        auto& modelEnitties = activeScene->GetComponentsOfType<Graphics::RZModel>();
        if (!modelEnitties.size()) {
            // Avocado
            auto& armadilloModelEntity = activeScene->createEntity("Avocado");
            armadilloModelEntity.AddComponent<Graphics::RZModel>("//Meshes/Avocado.gltf");
        }

        QMetaObject::invokeMethod(qrzeditorApp, [] {
            sceneHierarchyPanel->populateHierarchy();
        });
    }

    void RAZIX_CALL OnResize(uint32_t width, uint32_t height) override
    {
        std::lock_guard<std::mutex> lk(RZApplication::m);
        RZApplication::ready_for_execution = true;
        std::cout << "Triggering worker thread to resume execution :::: " << std::endl;
        RZApplication::halt_execution.notify_one();

        RAZIX_TRACE("Window Resize override Editor application! | W : {0}, H : {1}", width, height);
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Editor Application");
    return new RazixEditorApp();
}

// TODO: move the ifdef for platforms defined in EntryPoint to here and to Sandbox
HINSTANCE razixDll;

void LoadEngineDLL(int argc, char** argv)
{
    razixDll = LoadLibrary(L"Razix.dll");
    EngineMain(argc, argv);
}

int main(int argc, char** argv)
{
    // Initialize the QT Editor Application
    qrzeditorApp = new QApplication(argc, argv);

    QStyle* style = StyleData::availStyles[1].creator();
    QApplication::setStyle(style);
    qrzeditorApp->setWindowIcon(QIcon(":/rzeditor/RazixLogo64.png"));

    mainWindow = new Razix::Editor::RZEMainWindow;
    mainWindow->resize(1280, 720);
    mainWindow->setWindowTitle("Razix Engine Editor");

    sceneHierarchyPanel = new Razix::Editor::RZESceneHierarchyPanel;

    inspectorWidget = new Razix::Editor::RZEInspectorWindow(sceneHierarchyPanel);
    viewportWidget  = new Razix::Editor::RZEViewport;
    viewportWidget->resize(1280, 720);
    viewportWidget->setWindowTitle("Viewport");
    inspectorWidget->setWindowTitle("Inspector");
    inspectorWidget->setAutoFillBackground(true);
    QIcon razixIcon(":/rzeditor/RazixLogo64.png");
    inspectorWidget->setWindowIcon(razixIcon);
    viewportWidget->setWindowIcon(razixIcon);

    mainWindow->getToolWindowManager()->addToolWindow(inspectorWidget, ToolWindowManager::AreaReference(ToolWindowManager::LastUsedArea));
    mainWindow->getToolWindowManager()->addToolWindow(viewportWidget, ToolWindowManager::AreaReference(ToolWindowManager::LastUsedArea /*ToolWindowManager::AddTo, mainWindow->getToolWindowManager()->areaOf(inspectorWidget))*/));

    vulkanWindow = new Razix::Editor::RZEVulkanWindow;
    //vulkanWindow->show();
    auto vulkanWindowWidget = QWidget::createWindowContainer(vulkanWindow);

    vulkanWindowWidget->setWindowIcon(razixIcon);
    vulkanWindowWidget->setWindowTitle("Vulkan Window");

    // Scene Hierarchy Panel
    mainWindow->getToolWindowManager()->addToolWindow(sceneHierarchyPanel, ToolWindowManager::AreaReference(ToolWindowManager::LeftOf, mainWindow->getToolWindowManager()->areaOf(inspectorWidget)));

    // In order for event filter to work this is fookin important
    qrzeditorApp->installEventFilter(vulkanWindow);

    mainWindow->getToolWindowManager()->addToolWindow(vulkanWindowWidget, ToolWindowManager::AreaReference(ToolWindowManager::AddTo, mainWindow->getToolWindowManager()->areaOf(inspectorWidget)));
    // Load the engine DLL and Ignite it on a separate thread
    std::thread engineThread(LoadEngineDLL, argc, argv);
    engineThread.detach();

    mainWindow->show();

    return qrzeditorApp->exec();
}