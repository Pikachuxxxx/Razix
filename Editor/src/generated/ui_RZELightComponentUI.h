/********************************************************************************
** Form generated from reading UI file 'RZELightComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZELIGHTCOMPONENTUI_H
#define UI_RZELIGHTCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LightComponent
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *Directoinal_rb;
    QRadioButton *Point_rb;
    QRadioButton *Spot_rb;
    QSpacerItem *verticalSpacer;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *ColorLineEdit;
    QLabel *label_2;
    QLineEdit *RadiusLineEdit;

    void setupUi(QWidget *LightComponent)
    {
        if (LightComponent->objectName().isEmpty())
            LightComponent->setObjectName(QString::fromUtf8("LightComponent"));
        LightComponent->resize(344, 122);
        horizontalLayout = new QHBoxLayout(LightComponent);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(LightComponent);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        Directoinal_rb = new QRadioButton(groupBox);
        Directoinal_rb->setObjectName(QString::fromUtf8("Directoinal_rb"));

        verticalLayout->addWidget(Directoinal_rb);

        Point_rb = new QRadioButton(groupBox);
        Point_rb->setObjectName(QString::fromUtf8("Point_rb"));

        verticalLayout->addWidget(Point_rb);

        Spot_rb = new QRadioButton(groupBox);
        Spot_rb->setObjectName(QString::fromUtf8("Spot_rb"));

        verticalLayout->addWidget(Spot_rb);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_2->addWidget(groupBox);


        horizontalLayout->addLayout(verticalLayout_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(LightComponent);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        ColorLineEdit = new QLineEdit(LightComponent);
        ColorLineEdit->setObjectName(QString::fromUtf8("ColorLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, ColorLineEdit);

        label_2 = new QLabel(LightComponent);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        RadiusLineEdit = new QLineEdit(LightComponent);
        RadiusLineEdit->setObjectName(QString::fromUtf8("RadiusLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, RadiusLineEdit);


        horizontalLayout->addLayout(formLayout);


        retranslateUi(LightComponent);

        QMetaObject::connectSlotsByName(LightComponent);
    } // setupUi

    void retranslateUi(QWidget *LightComponent)
    {
        LightComponent->setWindowTitle(QCoreApplication::translate("LightComponent", "LightComponent", nullptr));
        groupBox->setTitle(QCoreApplication::translate("LightComponent", "Light Type", nullptr));
        Directoinal_rb->setText(QCoreApplication::translate("LightComponent", "Directional", nullptr));
        Point_rb->setText(QCoreApplication::translate("LightComponent", "Point", nullptr));
        Spot_rb->setText(QCoreApplication::translate("LightComponent", "Spot", nullptr));
        label->setText(QCoreApplication::translate("LightComponent", "Color", nullptr));
        label_2->setText(QCoreApplication::translate("LightComponent", "Radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LightComponent: public Ui_LightComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZELIGHTCOMPONENTUI_H
