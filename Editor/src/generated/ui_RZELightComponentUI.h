/********************************************************************************
** Form generated from reading UI file 'RZELightComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
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
    QPushButton *LightColor;
    QLabel *label_3;
    QLabel *label_2;
    QLineEdit *Radius;
    QLineEdit *Intensity;
    QButtonGroup *lightTypeGroup;

    void setupUi(QWidget *LightComponent)
    {
        if (LightComponent->objectName().isEmpty())
            LightComponent->setObjectName(QString::fromUtf8("LightComponent"));
        LightComponent->resize(473, 187);
        verticalLayout_2 = new QVBoxLayout(LightComponent);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(LightComponent);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 50));
        groupBox->setMaximumSize(QSize(16777215, 50));
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

        LightColor = new QPushButton(LightComponent);
        LightColor->setObjectName(QString::fromUtf8("LightColor"));
        LightColor->setFlat(false);

        formLayout->setWidget(1, QFormLayout::FieldRole, LightColor);

        label_3 = new QLabel(LightComponent);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        label_2 = new QLabel(LightComponent);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        Radius = new QLineEdit(LightComponent);
        Radius->setObjectName(QString::fromUtf8("Radius"));

        formLayout->setWidget(2, QFormLayout::FieldRole, Radius);

        Intensity = new QLineEdit(LightComponent);
        Intensity->setObjectName(QString::fromUtf8("Intensity"));

        formLayout->setWidget(0, QFormLayout::FieldRole, Intensity);


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
        LightColor->setText(QString());
        label_3->setText(QCoreApplication::translate("LightComponent", "Intensity", nullptr));
        label_2->setText(QCoreApplication::translate("LightComponent", "Radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LightComponent: public Ui_LightComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZELIGHTCOMPONENTUI_H
