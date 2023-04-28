/********************************************************************************
** Form generated from reading UI file 'RZELuaScriptComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZELUASCRIPTCOMPONENTUI_H
#define UI_RZELUASCRIPTCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LuaScriptComponent
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLineEdit *ScriptFileLineEdit;
    QPushButton *Browse;

    void setupUi(QWidget *LuaScriptComponent)
    {
        if (LuaScriptComponent->objectName().isEmpty())
            LuaScriptComponent->setObjectName(QString::fromUtf8("LuaScriptComponent"));
        LuaScriptComponent->resize(234, 43);
        formLayout = new QFormLayout(LuaScriptComponent);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(LuaScriptComponent);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ScriptFileLineEdit = new QLineEdit(LuaScriptComponent);
        ScriptFileLineEdit->setObjectName(QString::fromUtf8("ScriptFileLineEdit"));
        ScriptFileLineEdit->setMinimumSize(QSize(100, 0));
        ScriptFileLineEdit->setReadOnly(true);

        horizontalLayout->addWidget(ScriptFileLineEdit);

        Browse = new QPushButton(LuaScriptComponent);
        Browse->setObjectName(QString::fromUtf8("Browse"));

        horizontalLayout->addWidget(Browse);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout);


        retranslateUi(LuaScriptComponent);

        QMetaObject::connectSlotsByName(LuaScriptComponent);
    } // setupUi

    void retranslateUi(QWidget *LuaScriptComponent)
    {
        LuaScriptComponent->setWindowTitle(QCoreApplication::translate("LuaScriptComponent", "LuaScriptComponent", nullptr));
        label->setText(QCoreApplication::translate("LuaScriptComponent", "Script", nullptr));
        ScriptFileLineEdit->setPlaceholderText(QCoreApplication::translate("LuaScriptComponent", "Lua Script file ", nullptr));
        Browse->setText(QCoreApplication::translate("LuaScriptComponent", "Browse", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LuaScriptComponent: public Ui_LuaScriptComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZELUASCRIPTCOMPONENTUI_H
