/********************************************************************************
** Form generated from reading UI file 'RZERendererSettingsUIWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZERENDERERSETTINGSUIWINDOW_H
#define UI_RZERENDERERSETTINGSUIWINDOW_H

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
    QCheckBox *useProcSkyboxChkBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QFrame *line_3;
    QComboBox *tonemapMode;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QFrame *line_4;
    QComboBox *aaMode;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QFrame *line_5;
    QComboBox *sceneSamplingPattern;
    QSpacerItem *horizontalSpacer_4;
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
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *checkBox_7;
    QCheckBox *checkBox_8;
    QCheckBox *checkBox_9;
    QCheckBox *debugflag_visCSM;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_2;
    QCheckBox *checkBox_2;
    QCheckBox *Shadows;
    QCheckBox *Skybox;
    QCheckBox *SSAO;
    QCheckBox *TAA;
    QCheckBox *enableTonemapping;
    QCheckBox *FXAA;
    QCheckBox *ImGui;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *WorldSettings)
    {
        if (WorldSettings->objectName().isEmpty())
            WorldSettings->setObjectName(QString::fromUtf8("WorldSettings"));
        WorldSettings->resize(839, 795);
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

        useProcSkyboxChkBox = new QCheckBox(WorldSettings);
        useProcSkyboxChkBox->setObjectName(QString::fromUtf8("useProcSkyboxChkBox"));
        QFont font1;
        font1.setPointSize(10);
        useProcSkyboxChkBox->setFont(font1);
        useProcSkyboxChkBox->setChecked(false);

        verticalLayout->addWidget(useProcSkyboxChkBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(WorldSettings);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(200, 0));
        label->setMaximumSize(QSize(200, 16777215));
        label->setFont(font1);

        horizontalLayout_2->addWidget(label);

        line_3 = new QFrame(WorldSettings);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line_3);

        tonemapMode = new QComboBox(WorldSettings);
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


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_4 = new QLabel(WorldSettings);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(200, 0));
        label_4->setMaximumSize(QSize(200, 16777215));
        label_4->setFont(font1);

        horizontalLayout_3->addWidget(label_4);

        line_4 = new QFrame(WorldSettings);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(line_4);

        aaMode = new QComboBox(WorldSettings);
        aaMode->addItem(QString());
        aaMode->addItem(QString());
        aaMode->addItem(QString());
        aaMode->setObjectName(QString::fromUtf8("aaMode"));

        horizontalLayout_3->addWidget(aaMode);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(WorldSettings);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(200, 0));
        label_5->setMaximumSize(QSize(200, 16777215));
        label_5->setFont(font1);

        horizontalLayout_4->addWidget(label_5);

        line_5 = new QFrame(WorldSettings);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setFrameShape(QFrame::VLine);
        line_5->setFrameShadow(QFrame::Sunken);

        horizontalLayout_4->addWidget(line_5);

        sceneSamplingPattern = new QComboBox(WorldSettings);
        sceneSamplingPattern->addItem(QString());
        sceneSamplingPattern->addItem(QString());
        sceneSamplingPattern->addItem(QString());
        sceneSamplingPattern->setObjectName(QString::fromUtf8("sceneSamplingPattern"));

        horizontalLayout_4->addWidget(sceneSamplingPattern);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);

        groupBox = new QGroupBox(WorldSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Fira Code Light"));
        font2.setPointSize(10);
        groupBox->setFont(font2);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        filterRadiusLbl = new QLabel(groupBox);
        filterRadiusLbl->setObjectName(QString::fromUtf8("filterRadiusLbl"));
        filterRadiusLbl->setMinimumSize(QSize(200, 0));
        filterRadiusLbl->setMaximumSize(QSize(200, 16777215));
        filterRadiusLbl->setFont(font2);

        verticalLayout_2->addWidget(filterRadiusLbl);

        strengthLbl = new QLabel(groupBox);
        strengthLbl->setObjectName(QString::fromUtf8("strengthLbl"));
        strengthLbl->setMinimumSize(QSize(200, 0));
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

        groupBox_3 = new QGroupBox(WorldSettings);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font2);
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

        debugflag_visCSM = new QCheckBox(groupBox_3);
        debugflag_visCSM->setObjectName(QString::fromUtf8("debugflag_visCSM"));

        verticalLayout_4->addWidget(debugflag_visCSM);


        verticalLayout->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(WorldSettings);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setFont(font2);
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

        Skybox = new QCheckBox(groupBox_4);
        Skybox->setObjectName(QString::fromUtf8("Skybox"));
        Skybox->setChecked(true);

        verticalLayout_5->addWidget(Skybox);

        SSAO = new QCheckBox(groupBox_4);
        SSAO->setObjectName(QString::fromUtf8("SSAO"));
        SSAO->setChecked(true);

        verticalLayout_5->addWidget(SSAO);

        TAA = new QCheckBox(groupBox_4);
        TAA->setObjectName(QString::fromUtf8("TAA"));

        verticalLayout_5->addWidget(TAA);

        enableTonemapping = new QCheckBox(groupBox_4);
        enableTonemapping->setObjectName(QString::fromUtf8("enableTonemapping"));
        enableTonemapping->setChecked(true);

        verticalLayout_5->addWidget(enableTonemapping);

        FXAA = new QCheckBox(groupBox_4);
        FXAA->setObjectName(QString::fromUtf8("FXAA"));

        verticalLayout_5->addWidget(FXAA);

        ImGui = new QCheckBox(groupBox_4);
        ImGui->setObjectName(QString::fromUtf8("ImGui"));
        ImGui->setChecked(true);

        verticalLayout_5->addWidget(ImGui);

        Skybox->raise();
        label_2->raise();
        checkBox_2->raise();
        Shadows->raise();
        SSAO->raise();
        enableTonemapping->raise();
        FXAA->raise();
        TAA->raise();
        ImGui->raise();

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
        useProcSkyboxChkBox->setText(QCoreApplication::translate("WorldSettings", "Use Procedural Skybox", nullptr));
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

        label_4->setText(QCoreApplication::translate("WorldSettings", "Anti-Aliasing", nullptr));
        aaMode->setItemText(0, QCoreApplication::translate("WorldSettings", "NoAA", nullptr));
        aaMode->setItemText(1, QCoreApplication::translate("WorldSettings", "FXAA", nullptr));
        aaMode->setItemText(2, QCoreApplication::translate("WorldSettings", "TAA", nullptr));

        label_5->setText(QCoreApplication::translate("WorldSettings", "Scene Sampling pattern", nullptr));
        sceneSamplingPattern->setItemText(0, QCoreApplication::translate("WorldSettings", "Normal", nullptr));
        sceneSamplingPattern->setItemText(1, QCoreApplication::translate("WorldSettings", "Halton", nullptr));
        sceneSamplingPattern->setItemText(2, QCoreApplication::translate("WorldSettings", "Stratified", nullptr));

        groupBox->setTitle(QCoreApplication::translate("WorldSettings", "Bloom Settings", nullptr));
        filterRadiusLbl->setText(QCoreApplication::translate("WorldSettings", "filterRadius", nullptr));
        strengthLbl->setText(QCoreApplication::translate("WorldSettings", "strength", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("WorldSettings", "Debug Mode Flags", nullptr));
        checkBox_7->setText(QCoreApplication::translate("WorldSettings", "Quad Overdraw", nullptr));
        checkBox_8->setText(QCoreApplication::translate("WorldSettings", "Unlit Shading", nullptr));
        checkBox_9->setText(QCoreApplication::translate("WorldSettings", "Show UV coords", nullptr));
        debugflag_visCSM->setText(QCoreApplication::translate("WorldSettings", "Vis CSM Cascades", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("WorldSettings", "Renderer Features (Passes List)", nullptr));
        label_2->setText(QCoreApplication::translate("WorldSettings", "Flags", nullptr));
        checkBox_2->setText(QCoreApplication::translate("WorldSettings", "HiZ depth pre-pass", nullptr));
        Shadows->setText(QCoreApplication::translate("WorldSettings", "Shadows", nullptr));
        Skybox->setText(QCoreApplication::translate("WorldSettings", "Skybox", nullptr));
        SSAO->setText(QCoreApplication::translate("WorldSettings", "SSAO", nullptr));
        TAA->setText(QCoreApplication::translate("WorldSettings", "TAA", nullptr));
        enableTonemapping->setText(QCoreApplication::translate("WorldSettings", "Tonemapping", nullptr));
        FXAA->setText(QCoreApplication::translate("WorldSettings", "FXAA", nullptr));
        ImGui->setText(QCoreApplication::translate("WorldSettings", "ImGui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WorldSettings: public Ui_WorldSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZERENDERERSETTINGSUIWINDOW_H
