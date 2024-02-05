/********************************************************************************
** Form generated from reading UI file 'RZESpriteRendererComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZESPRITERENDERERCOMPONENTUI_H
#define UI_RZESPRITERENDERERCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SpriteRendererComponent
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QPushButton *sprite;
    QLabel *label_2;
    QLineEdit *colorLineEdit;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QCheckBox *FlipX;
    QCheckBox *FlipY;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *SpriteRendererComponent)
    {
        if (SpriteRendererComponent->objectName().isEmpty())
            SpriteRendererComponent->setObjectName(QString::fromUtf8("SpriteRendererComponent"));
        SpriteRendererComponent->resize(169, 148);
        formLayout = new QFormLayout(SpriteRendererComponent);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(SpriteRendererComponent);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        sprite = new QPushButton(SpriteRendererComponent);
        sprite->setObjectName(QString::fromUtf8("sprite"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sprite->sizePolicy().hasHeightForWidth());
        sprite->setSizePolicy(sizePolicy);
        sprite->setMinimumSize(QSize(75, 75));
        sprite->setFlat(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, sprite);

        label_2 = new QLabel(SpriteRendererComponent);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        colorLineEdit = new QLineEdit(SpriteRendererComponent);
        colorLineEdit->setObjectName(QString::fromUtf8("colorLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, colorLineEdit);

        label_3 = new QLabel(SpriteRendererComponent);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        FlipX = new QCheckBox(SpriteRendererComponent);
        FlipX->setObjectName(QString::fromUtf8("FlipX"));

        horizontalLayout->addWidget(FlipX);

        FlipY = new QCheckBox(SpriteRendererComponent);
        FlipY->setObjectName(QString::fromUtf8("FlipY"));

        horizontalLayout->addWidget(FlipY);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);


        retranslateUi(SpriteRendererComponent);

        QMetaObject::connectSlotsByName(SpriteRendererComponent);
    } // setupUi

    void retranslateUi(QWidget *SpriteRendererComponent)
    {
        SpriteRendererComponent->setWindowTitle(QCoreApplication::translate("SpriteRendererComponent", "SpriteRendererComponent", nullptr));
        label->setText(QCoreApplication::translate("SpriteRendererComponent", "Sprite", nullptr));
        sprite->setText(QString());
        label_2->setText(QCoreApplication::translate("SpriteRendererComponent", "Color", nullptr));
        label_3->setText(QCoreApplication::translate("SpriteRendererComponent", "Flip", nullptr));
        FlipX->setText(QCoreApplication::translate("SpriteRendererComponent", "X", nullptr));
        FlipY->setText(QCoreApplication::translate("SpriteRendererComponent", "Y", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SpriteRendererComponent: public Ui_SpriteRendererComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZESPRITERENDERERCOMPONENTUI_H
