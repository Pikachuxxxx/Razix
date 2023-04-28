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
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Viewport
{
public:
    QVBoxLayout *centralVLayout;

    void setupUi(QWidget *Viewport)
    {
        if (Viewport->objectName().isEmpty())
            Viewport->setObjectName(QString::fromUtf8("Viewport"));
        Viewport->resize(761, 608);
        Viewport->setStyleSheet(QString::fromUtf8("border: 20px;\n"
"border-color: rgb(255, 0, 4);"));
        centralVLayout = new QVBoxLayout(Viewport);
        centralVLayout->setObjectName(QString::fromUtf8("centralVLayout"));

        retranslateUi(Viewport);

        QMetaObject::connectSlotsByName(Viewport);
    } // setupUi

    void retranslateUi(QWidget *Viewport)
    {
        Viewport->setWindowTitle(QCoreApplication::translate("Viewport", "Viewport", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Viewport: public Ui_Viewport {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEVIEWPORT_H
