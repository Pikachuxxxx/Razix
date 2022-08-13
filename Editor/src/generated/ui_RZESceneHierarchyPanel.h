/********************************************************************************
** Form generated from reading UI file 'RZESceneHierarchyPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZESCENEHIERARCHYPANEL_H
#define UI_RZESCENEHIERARCHYPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SceneHierarchyPanel
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidget *sceneTree;

    void setupUi(QFrame *SceneHierarchyPanel)
    {
        if (SceneHierarchyPanel->objectName().isEmpty())
            SceneHierarchyPanel->setObjectName(QString::fromUtf8("SceneHierarchyPanel"));
        SceneHierarchyPanel->resize(438, 767);
        SceneHierarchyPanel->setStyleSheet(QString::fromUtf8("QTreeView::branch:has-siblings:!adjoins-item {\n"
"    border-image: url(:/rzeditor/vline.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-more.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:!has-children:!has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-end.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:has-children:!has-siblings:closed,\n"
"QTreeView::branch:closed:has-children:has-siblings {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-closed.png);\n"
"}\n"
"\n"
"QTreeView::branch:open:has-children:!has-siblings,\n"
"QTreeView::branch:open:has-children:has-siblings  {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-open.png);\n"
"}"));
        verticalLayout = new QVBoxLayout(SceneHierarchyPanel);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        sceneTree = new QTreeWidget(SceneHierarchyPanel);
        sceneTree->setObjectName(QString::fromUtf8("sceneTree"));
        sceneTree->setStyleSheet(QString::fromUtf8("QTreeWidget::branch:has-siblings:!adjoins-item {\n"
"    border-image: url(:/rzeditor/vline.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-more.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:!has-children:!has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-end.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:has-children:!has-siblings:closed,\n"
"QTreeWidget::branch:closed:has-children:has-siblings {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-closed.png);\n"
"}\n"
"\n"
"QTreeWidget::branch:open:has-children:!has-siblings,\n"
"QTreeWidget::branch:open:has-children:has-siblings  {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-open.png);\n"
"}"));

        verticalLayout->addWidget(sceneTree);


        retranslateUi(SceneHierarchyPanel);

        QMetaObject::connectSlotsByName(SceneHierarchyPanel);
    } // setupUi

    void retranslateUi(QFrame *SceneHierarchyPanel)
    {
        SceneHierarchyPanel->setWindowTitle(QCoreApplication::translate("SceneHierarchyPanel", "Scene Hierarchy Panel", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = sceneTree->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("SceneHierarchyPanel", "Entity List", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneHierarchyPanel: public Ui_SceneHierarchyPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZESCENEHIERARCHYPANEL_H
