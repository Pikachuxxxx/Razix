/********************************************************************************
** Form generated from reading UI file 'RZECameraComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZECAMERACOMPONENTUI_H
#define UI_RZECAMERACOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraComponent
{
public:
    QFormLayout *formLayout_2;
    QLabel *label;
    QWidget *colorPickerPlaceholder;
    QLabel *label_2;
    QComboBox *comboBox;
    QLabel *label_3;
    QSlider *horizontalSlider;
    QLabel *label_4;
    QCheckBox *checkBox;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QLabel *label_6;
    QLineEdit *lineEdit_2;

    void setupUi(QWidget *CameraComponent)
    {
        if (CameraComponent->objectName().isEmpty())
            CameraComponent->setObjectName(QString::fromUtf8("CameraComponent"));
        CameraComponent->resize(341, 259);
        formLayout_2 = new QFormLayout(CameraComponent);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label = new QLabel(CameraComponent);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        colorPickerPlaceholder = new QWidget(CameraComponent);
        colorPickerPlaceholder->setObjectName(QString::fromUtf8("colorPickerPlaceholder"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, colorPickerPlaceholder);

        label_2 = new QLabel(CameraComponent);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        comboBox = new QComboBox(CameraComponent);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, comboBox);

        label_3 = new QLabel(CameraComponent);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_3);

        horizontalSlider = new QSlider(CameraComponent);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setMaximum(180);
        horizontalSlider->setPageStep(5);
        horizontalSlider->setValue(60);
        horizontalSlider->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, horizontalSlider);

        label_4 = new QLabel(CameraComponent);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_4);

        checkBox = new QCheckBox(CameraComponent);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, checkBox);

        label_5 = new QLabel(CameraComponent);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_5);

        lineEdit = new QLineEdit(CameraComponent);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, lineEdit);

        label_6 = new QLabel(CameraComponent);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_6);

        lineEdit_2 = new QLineEdit(CameraComponent);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, lineEdit_2);


        retranslateUi(CameraComponent);

        QMetaObject::connectSlotsByName(CameraComponent);
    } // setupUi

    void retranslateUi(QWidget *CameraComponent)
    {
        CameraComponent->setWindowTitle(QCoreApplication::translate("CameraComponent", "Camera Component", nullptr));
        label->setText(QCoreApplication::translate("CameraComponent", "Background", nullptr));
        label_2->setText(QCoreApplication::translate("CameraComponent", "Projection", nullptr));
        label_3->setText(QCoreApplication::translate("CameraComponent", "FOV", nullptr));
        label_4->setText(QCoreApplication::translate("CameraComponent", "Primary Camera", nullptr));
        checkBox->setText(QString());
        label_5->setText(QCoreApplication::translate("CameraComponent", "Near Clip", nullptr));
        label_6->setText(QCoreApplication::translate("CameraComponent", "Far Clip", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CameraComponent: public Ui_CameraComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZECAMERACOMPONENTUI_H
