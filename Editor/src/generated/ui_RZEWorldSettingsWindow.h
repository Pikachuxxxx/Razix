/********************************************************************************
** Form generated from reading UI file 'RZEWorldSettingsWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEWORLDSETTINGSWINDOW_H
#define UI_RZEWORLDSETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WorldSettings
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *useProcSkyboxChkBox;
    QFrame *line_6;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *filterRadiusLbl;
    QLabel *strengthLbl;
    QFrame *line;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *filterRadius;
    QLineEdit *strength;
    QSpacerItem *horizontalSpacer;
    QFrame *line_2;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QFrame *line_3;
    QComboBox *tonemapMode;
    QSpacerItem *horizontalSpacer_2;
    QFrame *line_4;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *checkBox_7;
    QCheckBox *checkBox_8;
    QCheckBox *checkBox_9;
    QFrame *line_5;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_2;
    QCheckBox *checkBox_2;
    QCheckBox *Shadows;
    QCheckBox *ImGui;
    QCheckBox *SSAO;
    QCheckBox *checkBox_5;
    QCheckBox *checkBox_6;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *WorldSettings)
    {
        if (WorldSettings->objectName().isEmpty())
            WorldSettings->setObjectName(QString::fromUtf8("WorldSettings"));
        WorldSettings->resize(290, 759);
        verticalLayout = new QVBoxLayout(WorldSettings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_3 = new QLabel(WorldSettings);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font;
        font.setFamily(QString::fromUtf8("Fira Code SemiBold"));
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        verticalLayout->addWidget(label_3);

        groupBox_5 = new QGroupBox(WorldSettings);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Fira Code Light"));
        font1.setPointSize(10);
        groupBox_5->setFont(font1);
        verticalLayout_6 = new QVBoxLayout(groupBox_5);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        useProcSkyboxChkBox = new QCheckBox(groupBox_5);
        useProcSkyboxChkBox->setObjectName(QString::fromUtf8("useProcSkyboxChkBox"));
        useProcSkyboxChkBox->setChecked(false);

        verticalLayout_6->addWidget(useProcSkyboxChkBox);


        verticalLayout->addWidget(groupBox_5);

        line_6 = new QFrame(WorldSettings);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setStyleSheet(QString::fromUtf8("border-color:#000000;\n"
"border-style:solid;\n"
"border-width:2px;"));
        line_6->setFrameShadow(QFrame::Plain);
        line_6->setLineWidth(1);
        line_6->setFrameShape(QFrame::HLine);

        verticalLayout->addWidget(line_6);

        groupBox = new QGroupBox(WorldSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFont(font1);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        filterRadiusLbl = new QLabel(groupBox);
        filterRadiusLbl->setObjectName(QString::fromUtf8("filterRadiusLbl"));
        filterRadiusLbl->setMinimumSize(QSize(100, 0));
        filterRadiusLbl->setMaximumSize(QSize(200, 16777215));
        QFont font2;
        font2.setPointSize(8);
        filterRadiusLbl->setFont(font2);

        verticalLayout_2->addWidget(filterRadiusLbl);

        strengthLbl = new QLabel(groupBox);
        strengthLbl->setObjectName(QString::fromUtf8("strengthLbl"));
        strengthLbl->setMinimumSize(QSize(100, 0));
        strengthLbl->setMaximumSize(QSize(200, 16777215));
        strengthLbl->setFont(font2);

        verticalLayout_2->addWidget(strengthLbl);


        horizontalLayout->addLayout(verticalLayout_2);

        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        filterRadius = new QLineEdit(groupBox);
        filterRadius->setObjectName(QString::fromUtf8("filterRadius"));

        verticalLayout_3->addWidget(filterRadius);

        strength = new QLineEdit(groupBox);
        strength->setObjectName(QString::fromUtf8("strength"));

        verticalLayout_3->addWidget(strength);


        horizontalLayout->addLayout(verticalLayout_3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(groupBox);

        line_2 = new QFrame(WorldSettings);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setStyleSheet(QString::fromUtf8("border-color:#000000;\n"
"border-style:solid;\n"
"border-width:2px;"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        groupBox_2 = new QGroupBox(WorldSettings);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setFont(font1);
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(100, 0));
        label->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(label);

        line_3 = new QFrame(groupBox_2);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line_3);

        tonemapMode = new QComboBox(groupBox_2);
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->addItem(QString());
        tonemapMode->setObjectName(QString::fromUtf8("tonemapMode"));

        horizontalLayout_2->addWidget(tonemapMode);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(groupBox_2);

        line_4 = new QFrame(WorldSettings);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setStyleSheet(QString::fromUtf8("border-color:#000000;\n"
"border-style:solid;\n"
"border-width:2px;"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_4);

        groupBox_3 = new QGroupBox(WorldSettings);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font1);
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        checkBox_7 = new QCheckBox(groupBox_3);
        checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));
        checkBox_7->setAutoExclusive(true);

        verticalLayout_4->addWidget(checkBox_7);

        checkBox_8 = new QCheckBox(groupBox_3);
        checkBox_8->setObjectName(QString::fromUtf8("checkBox_8"));
        checkBox_8->setAutoExclusive(true);

        verticalLayout_4->addWidget(checkBox_8);

        checkBox_9 = new QCheckBox(groupBox_3);
        checkBox_9->setObjectName(QString::fromUtf8("checkBox_9"));
        checkBox_9->setAutoExclusive(true);

        verticalLayout_4->addWidget(checkBox_9);


        verticalLayout->addWidget(groupBox_3);

        line_5 = new QFrame(WorldSettings);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setStyleSheet(QString::fromUtf8("border-color:#000000;\n"
"border-style:solid;\n"
"border-width:2px;"));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_5);

        groupBox_4 = new QGroupBox(WorldSettings);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setFont(font1);
        verticalLayout_5 = new QVBoxLayout(groupBox_4);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_5->addWidget(label_2);

        checkBox_2 = new QCheckBox(groupBox_4);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        verticalLayout_5->addWidget(checkBox_2);

        Shadows = new QCheckBox(groupBox_4);
        Shadows->setObjectName(QString::fromUtf8("Shadows"));
        Shadows->setChecked(true);

        verticalLayout_5->addWidget(Shadows);

        ImGui = new QCheckBox(groupBox_4);
        ImGui->setObjectName(QString::fromUtf8("ImGui"));
        ImGui->setChecked(true);

        verticalLayout_5->addWidget(ImGui);

        SSAO = new QCheckBox(groupBox_4);
        SSAO->setObjectName(QString::fromUtf8("SSAO"));
        SSAO->setChecked(true);

        verticalLayout_5->addWidget(SSAO);

        checkBox_5 = new QCheckBox(groupBox_4);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

        verticalLayout_5->addWidget(checkBox_5);

        checkBox_6 = new QCheckBox(groupBox_4);
        checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));

        verticalLayout_5->addWidget(checkBox_6);


        verticalLayout->addWidget(groupBox_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(WorldSettings);

        QMetaObject::connectSlotsByName(WorldSettings);
    } // setupUi

    void retranslateUi(QWidget *WorldSettings)
    {
        WorldSettings->setWindowTitle(QCoreApplication::translate("WorldSettings", "World Settings", nullptr));
        label_3->setText(QCoreApplication::translate("WorldSettings", "World Settings", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("WorldSettings", "Environment Settings", nullptr));
        useProcSkyboxChkBox->setText(QCoreApplication::translate("WorldSettings", "Use Procedural Skybox", nullptr));
        groupBox->setTitle(QCoreApplication::translate("WorldSettings", "Bloom Settings", nullptr));
        filterRadiusLbl->setText(QCoreApplication::translate("WorldSettings", "filterRadius", nullptr));
        strengthLbl->setText(QCoreApplication::translate("WorldSettings", "strength", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("WorldSettings", "ToneMap Settings", nullptr));
        label->setText(QCoreApplication::translate("WorldSettings", "Tonemap Mode", nullptr));
        tonemapMode->setItemText(0, QCoreApplication::translate("WorldSettings", "ACES", nullptr));
        tonemapMode->setItemText(1, QCoreApplication::translate("WorldSettings", "Filmic", nullptr));
        tonemapMode->setItemText(2, QCoreApplication::translate("WorldSettings", "Lottes", nullptr));
        tonemapMode->setItemText(3, QCoreApplication::translate("WorldSettings", "Reinhard", nullptr));
        tonemapMode->setItemText(4, QCoreApplication::translate("WorldSettings", "Reinhard_V2", nullptr));
        tonemapMode->setItemText(5, QCoreApplication::translate("WorldSettings", "Uchimura", nullptr));
        tonemapMode->setItemText(6, QCoreApplication::translate("WorldSettings", "Uncharted 2", nullptr));
        tonemapMode->setItemText(7, QCoreApplication::translate("WorldSettings", "Unreal", nullptr));
        tonemapMode->setItemText(8, QCoreApplication::translate("WorldSettings", "None", nullptr));

        groupBox_3->setTitle(QCoreApplication::translate("WorldSettings", "Debug Mode Flags", nullptr));
        checkBox_7->setText(QCoreApplication::translate("WorldSettings", "Overdraw vis mode", nullptr));
        checkBox_8->setText(QCoreApplication::translate("WorldSettings", "Unlit mode", nullptr));
        checkBox_9->setText(QCoreApplication::translate("WorldSettings", "Show UV coords", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("WorldSettings", "Renderer Features (Passes List)", nullptr));
        label_2->setText(QCoreApplication::translate("WorldSettings", "Flags", nullptr));
        checkBox_2->setText(QCoreApplication::translate("WorldSettings", "GI", nullptr));
        Shadows->setText(QCoreApplication::translate("WorldSettings", "Shadows", nullptr));
        ImGui->setText(QCoreApplication::translate("WorldSettings", "ImGui", nullptr));
        SSAO->setText(QCoreApplication::translate("WorldSettings", "SSAO", nullptr));
        checkBox_5->setText(QCoreApplication::translate("WorldSettings", "SSR", nullptr));
        checkBox_6->setText(QCoreApplication::translate("WorldSettings", "Bloom", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WorldSettings: public Ui_WorldSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEWORLDSETTINGSWINDOW_H
