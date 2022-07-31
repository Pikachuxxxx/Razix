/********************************************************************************
** Form generated from reading UI file 'RZEInspectorWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEINSPECTORWINDOW_H
#define UI_RZEINSPECTORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InspectorWindow
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QWidget *InspectorWindow)
    {
        if (InspectorWindow->objectName().isEmpty())
            InspectorWindow->setObjectName(QString::fromUtf8("InspectorWindow"));
        InspectorWindow->resize(400, 486);
        InspectorWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 170, 127);"));
        pushButton = new QPushButton(InspectorWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(30, 20, 101, 41));
        pushButton_2 = new QPushButton(InspectorWindow);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(30, 70, 101, 41));
        pushButton_3 = new QPushButton(InspectorWindow);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(30, 120, 101, 41));

        retranslateUi(InspectorWindow);

        QMetaObject::connectSlotsByName(InspectorWindow);
    } // setupUi

    void retranslateUi(QWidget *InspectorWindow)
    {
        InspectorWindow->setWindowTitle(QCoreApplication::translate("InspectorWindow", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("InspectorWindow", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("InspectorWindow", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("InspectorWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InspectorWindow: public Ui_InspectorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEINSPECTORWINDOW_H
