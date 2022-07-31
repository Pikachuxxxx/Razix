/********************************************************************************
** Form generated from reading UI file 'RZEViewport.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEVIEWPORT_H
#define UI_RZEVIEWPORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Viewport
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;

    void setupUi(QWidget *Viewport)
    {
        if (Viewport->objectName().isEmpty())
            Viewport->setObjectName(QString::fromUtf8("Viewport"));
        Viewport->resize(400, 300);
        pushButton = new QPushButton(Viewport);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(80, 110, 75, 23));
        pushButton_2 = new QPushButton(Viewport);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(140, 170, 75, 23));
        pushButton_3 = new QPushButton(Viewport);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(190, 120, 75, 23));
        pushButton_4 = new QPushButton(Viewport);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(240, 70, 75, 23));

        retranslateUi(Viewport);

        QMetaObject::connectSlotsByName(Viewport);
    } // setupUi

    void retranslateUi(QWidget *Viewport)
    {
        Viewport->setWindowTitle(QCoreApplication::translate("Viewport", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("Viewport", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Viewport", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Viewport", "PushButton", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Viewport", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Viewport: public Ui_Viewport {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEVIEWPORT_H
