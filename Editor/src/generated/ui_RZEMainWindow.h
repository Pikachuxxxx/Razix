/********************************************************************************
** Form generated from reading UI file 'RZEMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEMAINWINDOW_H
#define UI_RZEMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLighting_Settings;
    QAction *actionEnvironment_Settings;
    QAction *actionEntity;
    QAction *actionID;
    QAction *actionTag;
    QAction *actionCamera;
    QAction *actionTransform;
    QAction *actionMeshRenderer;
    QAction *actionSpriteRenderer;
    QAction *actionLuaScript;
    QAction *actionLua_Script;
    QAction *actionPlane;
    QAction *actionCube;
    QAction *actionSphere;
    QAction *actionCylinder;
    QAction *actionTerrain;
    QAction *actionModel;
    QAction *actionTexture;
    QAction *actionShader;
    QAction *actionInspector;
    QAction *actionViewport;
    QAction *actionModel_2;
    QAction *actionLight;
    QAction *actionScene_Hierarchy;
    QAction *actionContent_Browser;
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionSave_Project;
    QAction *actionMaterial_Editor;
    QAction *actionSave_layout;
    QAction *actionRestore_layout;
    QAction *actionClear_layout;
    QAction *actionWorld_Renderer_Settings;
    QAction *actionProject_Settings;
    QAction *actionEngine_Settings;
    QAction *actionDebug_Modes;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuWindows;
    QMenu *menuGraphics;
    QMenu *menuCreate;
    QMenu *menuComponent;
    QMenu *menuPrimitive;
    QMenu *menuImport;
    QMenu *menulayout;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(799, 600);
        actionLighting_Settings = new QAction(MainWindow);
        actionLighting_Settings->setObjectName(QString::fromUtf8("actionLighting_Settings"));
        actionEnvironment_Settings = new QAction(MainWindow);
        actionEnvironment_Settings->setObjectName(QString::fromUtf8("actionEnvironment_Settings"));
        actionEntity = new QAction(MainWindow);
        actionEntity->setObjectName(QString::fromUtf8("actionEntity"));
        actionID = new QAction(MainWindow);
        actionID->setObjectName(QString::fromUtf8("actionID"));
        actionTag = new QAction(MainWindow);
        actionTag->setObjectName(QString::fromUtf8("actionTag"));
        actionCamera = new QAction(MainWindow);
        actionCamera->setObjectName(QString::fromUtf8("actionCamera"));
        actionTransform = new QAction(MainWindow);
        actionTransform->setObjectName(QString::fromUtf8("actionTransform"));
        actionMeshRenderer = new QAction(MainWindow);
        actionMeshRenderer->setObjectName(QString::fromUtf8("actionMeshRenderer"));
        actionSpriteRenderer = new QAction(MainWindow);
        actionSpriteRenderer->setObjectName(QString::fromUtf8("actionSpriteRenderer"));
        actionLuaScript = new QAction(MainWindow);
        actionLuaScript->setObjectName(QString::fromUtf8("actionLuaScript"));
        actionLua_Script = new QAction(MainWindow);
        actionLua_Script->setObjectName(QString::fromUtf8("actionLua_Script"));
        actionPlane = new QAction(MainWindow);
        actionPlane->setObjectName(QString::fromUtf8("actionPlane"));
        actionCube = new QAction(MainWindow);
        actionCube->setObjectName(QString::fromUtf8("actionCube"));
        actionSphere = new QAction(MainWindow);
        actionSphere->setObjectName(QString::fromUtf8("actionSphere"));
        actionCylinder = new QAction(MainWindow);
        actionCylinder->setObjectName(QString::fromUtf8("actionCylinder"));
        actionTerrain = new QAction(MainWindow);
        actionTerrain->setObjectName(QString::fromUtf8("actionTerrain"));
        actionModel = new QAction(MainWindow);
        actionModel->setObjectName(QString::fromUtf8("actionModel"));
        actionTexture = new QAction(MainWindow);
        actionTexture->setObjectName(QString::fromUtf8("actionTexture"));
        actionShader = new QAction(MainWindow);
        actionShader->setObjectName(QString::fromUtf8("actionShader"));
        actionInspector = new QAction(MainWindow);
        actionInspector->setObjectName(QString::fromUtf8("actionInspector"));
        actionViewport = new QAction(MainWindow);
        actionViewport->setObjectName(QString::fromUtf8("actionViewport"));
        actionModel_2 = new QAction(MainWindow);
        actionModel_2->setObjectName(QString::fromUtf8("actionModel_2"));
        actionLight = new QAction(MainWindow);
        actionLight->setObjectName(QString::fromUtf8("actionLight"));
        actionScene_Hierarchy = new QAction(MainWindow);
        actionScene_Hierarchy->setObjectName(QString::fromUtf8("actionScene_Hierarchy"));
        actionContent_Browser = new QAction(MainWindow);
        actionContent_Browser->setObjectName(QString::fromUtf8("actionContent_Browser"));
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionSave_Project = new QAction(MainWindow);
        actionSave_Project->setObjectName(QString::fromUtf8("actionSave_Project"));
        actionMaterial_Editor = new QAction(MainWindow);
        actionMaterial_Editor->setObjectName(QString::fromUtf8("actionMaterial_Editor"));
        actionSave_layout = new QAction(MainWindow);
        actionSave_layout->setObjectName(QString::fromUtf8("actionSave_layout"));
        actionRestore_layout = new QAction(MainWindow);
        actionRestore_layout->setObjectName(QString::fromUtf8("actionRestore_layout"));
        actionClear_layout = new QAction(MainWindow);
        actionClear_layout->setObjectName(QString::fromUtf8("actionClear_layout"));
        actionWorld_Renderer_Settings = new QAction(MainWindow);
        actionWorld_Renderer_Settings->setObjectName(QString::fromUtf8("actionWorld_Renderer_Settings"));
        actionProject_Settings = new QAction(MainWindow);
        actionProject_Settings->setObjectName(QString::fromUtf8("actionProject_Settings"));
        actionEngine_Settings = new QAction(MainWindow);
        actionEngine_Settings->setObjectName(QString::fromUtf8("actionEngine_Settings"));
        actionDebug_Modes = new QAction(MainWindow);
        actionDebug_Modes->setObjectName(QString::fromUtf8("actionDebug_Modes"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 799, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuWindows = new QMenu(menubar);
        menuWindows->setObjectName(QString::fromUtf8("menuWindows"));
        menuGraphics = new QMenu(menubar);
        menuGraphics->setObjectName(QString::fromUtf8("menuGraphics"));
        menuCreate = new QMenu(menubar);
        menuCreate->setObjectName(QString::fromUtf8("menuCreate"));
        menuComponent = new QMenu(menuCreate);
        menuComponent->setObjectName(QString::fromUtf8("menuComponent"));
        menuPrimitive = new QMenu(menuCreate);
        menuPrimitive->setObjectName(QString::fromUtf8("menuPrimitive"));
        menuImport = new QMenu(menuCreate);
        menuImport->setObjectName(QString::fromUtf8("menuImport"));
        menulayout = new QMenu(menubar);
        menulayout->setObjectName(QString::fromUtf8("menulayout"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuWindows->menuAction());
        menubar->addAction(menuGraphics->menuAction());
        menubar->addAction(menuCreate->menuAction());
        menubar->addAction(menulayout->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addAction(actionSave_Project);
        menuFile->addSeparator();
        menuEdit->addAction(actionProject_Settings);
        menuEdit->addAction(actionEngine_Settings);
        menuGraphics->addAction(actionLighting_Settings);
        menuGraphics->addAction(actionEnvironment_Settings);
        menuGraphics->addAction(actionDebug_Modes);
        menuCreate->addAction(actionEntity);
        menuCreate->addAction(actionLua_Script);
        menuCreate->addAction(menuPrimitive->menuAction());
        menuCreate->addAction(menuComponent->menuAction());
        menuCreate->addSeparator();
        menuCreate->addAction(menuImport->menuAction());
        menuComponent->addAction(actionCamera);
        menuComponent->addAction(actionLight);
        menuComponent->addAction(actionLuaScript);
        menuComponent->addAction(actionMeshRenderer);
        menuComponent->addAction(actionSpriteRenderer);
        menuPrimitive->addAction(actionPlane);
        menuPrimitive->addAction(actionCube);
        menuPrimitive->addAction(actionSphere);
        menuPrimitive->addAction(actionCylinder);
        menuPrimitive->addAction(actionTerrain);
        menuImport->addAction(actionModel);
        menuImport->addAction(actionTexture);
        menuImport->addAction(actionShader);
        menulayout->addAction(actionSave_layout);
        menulayout->addAction(actionRestore_layout);
        menulayout->addAction(actionClear_layout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionLighting_Settings->setText(QCoreApplication::translate("MainWindow", "Lighting Settings", nullptr));
        actionEnvironment_Settings->setText(QCoreApplication::translate("MainWindow", "Environment Settings", nullptr));
        actionEntity->setText(QCoreApplication::translate("MainWindow", "Entity", nullptr));
        actionID->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        actionTag->setText(QCoreApplication::translate("MainWindow", "Tag", nullptr));
        actionCamera->setText(QCoreApplication::translate("MainWindow", "Camera", nullptr));
        actionTransform->setText(QCoreApplication::translate("MainWindow", "Transform", nullptr));
        actionMeshRenderer->setText(QCoreApplication::translate("MainWindow", "MeshRenderer", nullptr));
        actionSpriteRenderer->setText(QCoreApplication::translate("MainWindow", "SpriteRenderer", nullptr));
        actionLuaScript->setText(QCoreApplication::translate("MainWindow", "LuaScript", nullptr));
        actionLua_Script->setText(QCoreApplication::translate("MainWindow", "Lua Script", nullptr));
        actionPlane->setText(QCoreApplication::translate("MainWindow", "Plane", nullptr));
        actionCube->setText(QCoreApplication::translate("MainWindow", "Cube", nullptr));
        actionSphere->setText(QCoreApplication::translate("MainWindow", "Sphere", nullptr));
        actionCylinder->setText(QCoreApplication::translate("MainWindow", "Cylinder", nullptr));
        actionTerrain->setText(QCoreApplication::translate("MainWindow", "Terrain", nullptr));
        actionModel->setText(QCoreApplication::translate("MainWindow", "Model", nullptr));
        actionTexture->setText(QCoreApplication::translate("MainWindow", "Texture", nullptr));
        actionShader->setText(QCoreApplication::translate("MainWindow", "Shader", nullptr));
        actionInspector->setText(QCoreApplication::translate("MainWindow", "Inspector", nullptr));
        actionViewport->setText(QCoreApplication::translate("MainWindow", "Viewport", nullptr));
        actionModel_2->setText(QCoreApplication::translate("MainWindow", "Model", nullptr));
        actionLight->setText(QCoreApplication::translate("MainWindow", "Light", nullptr));
        actionScene_Hierarchy->setText(QCoreApplication::translate("MainWindow", "Scene Hierarchy", nullptr));
        actionContent_Browser->setText(QCoreApplication::translate("MainWindow", "Content Browser", nullptr));
        actionNew_Project->setText(QCoreApplication::translate("MainWindow", "New Project", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project", nullptr));
        actionSave_Project->setText(QCoreApplication::translate("MainWindow", "Save Project", nullptr));
        actionMaterial_Editor->setText(QCoreApplication::translate("MainWindow", "Material Editor", nullptr));
        actionSave_layout->setText(QCoreApplication::translate("MainWindow", "Save layout", nullptr));
        actionRestore_layout->setText(QCoreApplication::translate("MainWindow", "Restore layout", nullptr));
        actionClear_layout->setText(QCoreApplication::translate("MainWindow", "Clear layout", nullptr));
        actionWorld_Renderer_Settings->setText(QCoreApplication::translate("MainWindow", "World Renderer Settings", nullptr));
        actionProject_Settings->setText(QCoreApplication::translate("MainWindow", "Project Settings", nullptr));
        actionEngine_Settings->setText(QCoreApplication::translate("MainWindow", "Engine Settings", nullptr));
        actionDebug_Modes->setText(QCoreApplication::translate("MainWindow", "Debug Modes", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        menuWindows->setTitle(QCoreApplication::translate("MainWindow", "Windows", nullptr));
        menuGraphics->setTitle(QCoreApplication::translate("MainWindow", "Graphics", nullptr));
        menuCreate->setTitle(QCoreApplication::translate("MainWindow", "Create", nullptr));
        menuComponent->setTitle(QCoreApplication::translate("MainWindow", "Add Component", nullptr));
        menuPrimitive->setTitle(QCoreApplication::translate("MainWindow", "Primitive", nullptr));
        menuImport->setTitle(QCoreApplication::translate("MainWindow", "Import", nullptr));
        menulayout->setTitle(QCoreApplication::translate("MainWindow", "Layout", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEMAINWINDOW_H
