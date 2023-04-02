/********************************************************************************
** Form generated from reading UI file 'RZEWorldRendererSettingsWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEWORLDRENDERERSETTINGSWINDOW_H
#define UI_RZEWORLDRENDERERSETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WorldRendererSettings
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *filterRadius;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *strength;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *WorldRendererSettings)
    {
        if (WorldRendererSettings->objectName().isEmpty())
            WorldRendererSettings->setObjectName(QString::fromUtf8("WorldRendererSettings"));
        WorldRendererSettings->resize(312, 400);
        verticalLayout = new QVBoxLayout(WorldRendererSettings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(WorldRendererSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        filterRadius = new QLineEdit(groupBox);
        filterRadius->setObjectName(QString::fromUtf8("filterRadius"));

        horizontalLayout->addWidget(filterRadius);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        strength = new QLineEdit(groupBox);
        strength->setObjectName(QString::fromUtf8("strength"));

        horizontalLayout_2->addWidget(strength);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(WorldRendererSettings);

        QMetaObject::connectSlotsByName(WorldRendererSettings);
    } // setupUi

    void retranslateUi(QWidget *WorldRendererSettings)
    {
        WorldRendererSettings->setWindowTitle(QCoreApplication::translate("WorldRendererSettings", "World Settings", nullptr));
        groupBox->setTitle(QCoreApplication::translate("WorldRendererSettings", "Bloom Settings", nullptr));
        label->setText(QCoreApplication::translate("WorldRendererSettings", "filterRadius", nullptr));
        label_2->setText(QCoreApplication::translate("WorldRendererSettings", "strength", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WorldRendererSettings: public Ui_WorldRendererSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEWORLDRENDERERSETTINGSWINDOW_H
