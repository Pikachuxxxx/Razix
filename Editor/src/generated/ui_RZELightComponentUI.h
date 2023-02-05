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
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LightComponent
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QRadioButton *Directional_rb;
    QRadioButton *Point_rb;
    QRadioButton *Spot_rb;
    QSpacerItem *horizontalSpacer;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *RadiusLineEdit;
    QPushButton *light_color;
    QButtonGroup *lightTypeGroup;

    void setupUi(QWidget *LightComponent)
    {
        if (LightComponent->objectName().isEmpty())
            LightComponent->setObjectName(QString::fromUtf8("LightComponent"));
        LightComponent->resize(245, 122);
        verticalLayout_2 = new QVBoxLayout(LightComponent);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(LightComponent);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFlat(false);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        Directional_rb = new QRadioButton(groupBox);
        lightTypeGroup = new QButtonGroup(LightComponent);
        lightTypeGroup->setObjectName(QString::fromUtf8("lightTypeGroup"));
        lightTypeGroup->addButton(Directional_rb);
        Directional_rb->setObjectName(QString::fromUtf8("Directional_rb"));
        Directional_rb->setChecked(true);

        horizontalLayout->addWidget(Directional_rb);

        Point_rb = new QRadioButton(groupBox);
        lightTypeGroup->addButton(Point_rb);
        Point_rb->setObjectName(QString::fromUtf8("Point_rb"));

        horizontalLayout->addWidget(Point_rb);

        Spot_rb = new QRadioButton(groupBox);
        lightTypeGroup->addButton(Spot_rb);
        Spot_rb->setObjectName(QString::fromUtf8("Spot_rb"));
        Spot_rb->setChecked(false);

        horizontalLayout->addWidget(Spot_rb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addWidget(groupBox);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(LightComponent);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        label_2 = new QLabel(LightComponent);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2);

        RadiusLineEdit = new QLineEdit(LightComponent);
        RadiusLineEdit->setObjectName(QString::fromUtf8("RadiusLineEdit"));

        formLayout->setWidget(3, QFormLayout::FieldRole, RadiusLineEdit);

        light_color = new QPushButton(LightComponent);
        light_color->setObjectName(QString::fromUtf8("light_color"));
        light_color->setFlat(false);

        formLayout->setWidget(1, QFormLayout::FieldRole, light_color);


        verticalLayout_2->addLayout(formLayout);


        retranslateUi(LightComponent);

        QMetaObject::connectSlotsByName(LightComponent);
    } // setupUi

    void retranslateUi(QWidget *LightComponent)
    {
        LightComponent->setWindowTitle(QCoreApplication::translate("LightComponent", "LightComponent", nullptr));
        groupBox->setTitle(QCoreApplication::translate("LightComponent", "Light Type", nullptr));
        Directional_rb->setText(QCoreApplication::translate("LightComponent", "Directional", nullptr));
        Point_rb->setText(QCoreApplication::translate("LightComponent", "Point", nullptr));
        Spot_rb->setText(QCoreApplication::translate("LightComponent", "Spot", nullptr));
        label->setText(QCoreApplication::translate("LightComponent", "Color", nullptr));
        label_2->setText(QCoreApplication::translate("LightComponent", "Radius", nullptr));
        light_color->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LightComponent: public Ui_LightComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZELIGHTCOMPONENTUI_H
