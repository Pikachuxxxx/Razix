/********************************************************************************
** Form generated from reading UI file 'RZEMaterialEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEMATERIALEDITOR_H
#define UI_RZEMATERIALEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MaterialEditor
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLabel *shaderNameLbl;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *RecompileBtn;
    QPushButton *EditBtn;
    QPushButton *ShowReflectionBtn;
    QSpacerItem *horizontalSpacer_9;
    QFrame *line;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLineEdit *materialName;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *diffuseTexture;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *useDiffuseTexure;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QPushButton *diffuseColor;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_6;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *specTexture;
    QVBoxLayout *verticalLayout_7;
    QCheckBox *useSpecTexture;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_10;
    QLineEdit *specIntensity;
    QSpacerItem *horizontalSpacer_6;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *normalTexture;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *checkBox_3;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *metallicTexture;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *checkBox_4;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_8;
    QLineEdit *metallicValue;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_12;
    QPushButton *roughnessTexture;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *checkBox_5;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_9;
    QLineEdit *roughnessValue;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_16;
    QPushButton *emissiveTexture;
    QVBoxLayout *verticalLayout_8;
    QCheckBox *checkBox_7;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_11;
    QLineEdit *emissiveIntensity;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *groupBox_8;
    QHBoxLayout *horizontalLayout_18;
    QPushButton *aoTexture;
    QVBoxLayout *verticalLayout_9;
    QCheckBox *checkBox_8;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_12;
    QLineEdit *aoIntensity;
    QSpacerItem *horizontalSpacer_8;

    void setupUi(QWidget *MaterialEditor)
    {
        if (MaterialEditor->objectName().isEmpty())
            MaterialEditor->setObjectName(QString::fromUtf8("MaterialEditor"));
        MaterialEditor->resize(310, 842);
        MaterialEditor->setStyleSheet(QString::fromUtf8("background-color: #151515;"));
        verticalLayout = new QVBoxLayout(MaterialEditor);
        verticalLayout->setSpacing(5);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        scrollArea = new QScrollArea(MaterialEditor);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setMinimumSize(QSize(300, 0));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 298, 830));
        scrollAreaWidgetContents->setMinimumSize(QSize(0, 0));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        shaderNameLbl = new QLabel(scrollAreaWidgetContents);
        shaderNameLbl->setObjectName(QString::fromUtf8("shaderNameLbl"));
        shaderNameLbl->setStyleSheet(QString::fromUtf8("border-color: rgb(147, 147, 147);\n"
"border:1px;\n"
"border-radius:5;\n"
"border-style:solid;"));

        horizontalLayout_2->addWidget(shaderNameLbl);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_2);

        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_3->addWidget(label_2);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_3->addWidget(label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        RecompileBtn = new QPushButton(scrollAreaWidgetContents);
        RecompileBtn->setObjectName(QString::fromUtf8("RecompileBtn"));

        horizontalLayout->addWidget(RecompileBtn);

        EditBtn = new QPushButton(scrollAreaWidgetContents);
        EditBtn->setObjectName(QString::fromUtf8("EditBtn"));

        horizontalLayout->addWidget(EditBtn);

        ShowReflectionBtn = new QPushButton(scrollAreaWidgetContents);
        ShowReflectionBtn->setObjectName(QString::fromUtf8("ShowReflectionBtn"));

        horizontalLayout->addWidget(ShowReflectionBtn);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_9);


        verticalLayout_3->addLayout(horizontalLayout);

        line = new QFrame(scrollAreaWidgetContents);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetFixedSize);
        label_4 = new QLabel(scrollAreaWidgetContents);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setMaximumSize(QSize(16777215, 50));

        horizontalLayout_3->addWidget(label_4);

        materialName = new QLineEdit(scrollAreaWidgetContents);
        materialName->setObjectName(QString::fromUtf8("materialName"));
        materialName->setMaximumSize(QSize(16777215, 50));
        materialName->setFrame(true);

        horizontalLayout_3->addWidget(materialName);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_3);

        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFlat(false);
        horizontalLayout_5 = new QHBoxLayout(groupBox);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        diffuseTexture = new QPushButton(groupBox);
        diffuseTexture->setObjectName(QString::fromUtf8("diffuseTexture"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(diffuseTexture->sizePolicy().hasHeightForWidth());
        diffuseTexture->setSizePolicy(sizePolicy1);
        diffuseTexture->setMinimumSize(QSize(50, 50));
        diffuseTexture->setIconSize(QSize(50, 50));
        diffuseTexture->setFlat(false);

        horizontalLayout_5->addWidget(diffuseTexture);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        useDiffuseTexure = new QCheckBox(groupBox);
        useDiffuseTexure->setObjectName(QString::fromUtf8("useDiffuseTexure"));

        verticalLayout_2->addWidget(useDiffuseTexure);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_4->addWidget(label_5);

        diffuseColor = new QPushButton(groupBox);
        diffuseColor->setObjectName(QString::fromUtf8("diffuseColor"));
        diffuseColor->setMinimumSize(QSize(100, 0));

        horizontalLayout_4->addWidget(diffuseColor);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout_2);


        verticalLayout_3->addWidget(groupBox);

        groupBox_6 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        horizontalLayout_14 = new QHBoxLayout(groupBox_6);
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        specTexture = new QPushButton(groupBox_6);
        specTexture->setObjectName(QString::fromUtf8("specTexture"));
        sizePolicy1.setHeightForWidth(specTexture->sizePolicy().hasHeightForWidth());
        specTexture->setSizePolicy(sizePolicy1);
        specTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_14->addWidget(specTexture);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        useSpecTexture = new QCheckBox(groupBox_6);
        useSpecTexture->setObjectName(QString::fromUtf8("useSpecTexture"));

        verticalLayout_7->addWidget(useSpecTexture);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_10 = new QLabel(groupBox_6);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_15->addWidget(label_10);

        specIntensity = new QLineEdit(groupBox_6);
        specIntensity->setObjectName(QString::fromUtf8("specIntensity"));

        horizontalLayout_15->addWidget(specIntensity);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_6);


        verticalLayout_7->addLayout(horizontalLayout_15);


        horizontalLayout_14->addLayout(verticalLayout_7);


        verticalLayout_3->addWidget(groupBox_6);

        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_3);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        normalTexture = new QPushButton(groupBox_3);
        normalTexture->setObjectName(QString::fromUtf8("normalTexture"));
        sizePolicy1.setHeightForWidth(normalTexture->sizePolicy().hasHeightForWidth());
        normalTexture->setSizePolicy(sizePolicy1);
        normalTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_8->addWidget(normalTexture);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        checkBox_3 = new QCheckBox(groupBox_3);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        verticalLayout_4->addWidget(checkBox_3);


        horizontalLayout_8->addLayout(verticalLayout_4);


        verticalLayout_3->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        horizontalLayout_10 = new QHBoxLayout(groupBox_4);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        metallicTexture = new QPushButton(groupBox_4);
        metallicTexture->setObjectName(QString::fromUtf8("metallicTexture"));
        sizePolicy1.setHeightForWidth(metallicTexture->sizePolicy().hasHeightForWidth());
        metallicTexture->setSizePolicy(sizePolicy1);
        metallicTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_10->addWidget(metallicTexture);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        checkBox_4 = new QCheckBox(groupBox_4);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

        verticalLayout_5->addWidget(checkBox_4);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_11->addWidget(label_8);

        metallicValue = new QLineEdit(groupBox_4);
        metallicValue->setObjectName(QString::fromUtf8("metallicValue"));

        horizontalLayout_11->addWidget(metallicValue);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_4);


        verticalLayout_5->addLayout(horizontalLayout_11);


        horizontalLayout_10->addLayout(verticalLayout_5);


        verticalLayout_3->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        horizontalLayout_12 = new QHBoxLayout(groupBox_5);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        roughnessTexture = new QPushButton(groupBox_5);
        roughnessTexture->setObjectName(QString::fromUtf8("roughnessTexture"));
        sizePolicy1.setHeightForWidth(roughnessTexture->sizePolicy().hasHeightForWidth());
        roughnessTexture->setSizePolicy(sizePolicy1);
        roughnessTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_12->addWidget(roughnessTexture);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        checkBox_5 = new QCheckBox(groupBox_5);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

        verticalLayout_6->addWidget(checkBox_5);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_13->addWidget(label_9);

        roughnessValue = new QLineEdit(groupBox_5);
        roughnessValue->setObjectName(QString::fromUtf8("roughnessValue"));

        horizontalLayout_13->addWidget(roughnessValue);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_5);


        verticalLayout_6->addLayout(horizontalLayout_13);


        horizontalLayout_12->addLayout(verticalLayout_6);


        verticalLayout_3->addWidget(groupBox_5);

        groupBox_7 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        horizontalLayout_16 = new QHBoxLayout(groupBox_7);
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        emissiveTexture = new QPushButton(groupBox_7);
        emissiveTexture->setObjectName(QString::fromUtf8("emissiveTexture"));
        sizePolicy1.setHeightForWidth(emissiveTexture->sizePolicy().hasHeightForWidth());
        emissiveTexture->setSizePolicy(sizePolicy1);
        emissiveTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_16->addWidget(emissiveTexture);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        checkBox_7 = new QCheckBox(groupBox_7);
        checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));

        verticalLayout_8->addWidget(checkBox_7);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_11 = new QLabel(groupBox_7);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_17->addWidget(label_11);

        emissiveIntensity = new QLineEdit(groupBox_7);
        emissiveIntensity->setObjectName(QString::fromUtf8("emissiveIntensity"));

        horizontalLayout_17->addWidget(emissiveIntensity);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_7);


        verticalLayout_8->addLayout(horizontalLayout_17);


        horizontalLayout_16->addLayout(verticalLayout_8);


        verticalLayout_3->addWidget(groupBox_7);

        groupBox_8 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        horizontalLayout_18 = new QHBoxLayout(groupBox_8);
        horizontalLayout_18->setSpacing(6);
        horizontalLayout_18->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        aoTexture = new QPushButton(groupBox_8);
        aoTexture->setObjectName(QString::fromUtf8("aoTexture"));
        sizePolicy1.setHeightForWidth(aoTexture->sizePolicy().hasHeightForWidth());
        aoTexture->setSizePolicy(sizePolicy1);
        aoTexture->setMinimumSize(QSize(50, 50));

        horizontalLayout_18->addWidget(aoTexture);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        checkBox_8 = new QCheckBox(groupBox_8);
        checkBox_8->setObjectName(QString::fromUtf8("checkBox_8"));

        verticalLayout_9->addWidget(checkBox_8);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setSpacing(6);
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        label_12 = new QLabel(groupBox_8);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_19->addWidget(label_12);

        aoIntensity = new QLineEdit(groupBox_8);
        aoIntensity->setObjectName(QString::fromUtf8("aoIntensity"));

        horizontalLayout_19->addWidget(aoIntensity);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_19->addItem(horizontalSpacer_8);


        verticalLayout_9->addLayout(horizontalLayout_19);


        horizontalLayout_18->addLayout(verticalLayout_9);


        verticalLayout_3->addWidget(groupBox_8);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(MaterialEditor);

        QMetaObject::connectSlotsByName(MaterialEditor);
    } // setupUi

    void retranslateUi(QWidget *MaterialEditor)
    {
        MaterialEditor->setWindowTitle(QCoreApplication::translate("MaterialEditor", "Material Editor", nullptr));
        label->setText(QCoreApplication::translate("MaterialEditor", "Shader :", nullptr));
        shaderNameLbl->setText(QCoreApplication::translate("MaterialEditor", "pbr_tiled_deferred.rzsf", nullptr));
        label_2->setText(QCoreApplication::translate("MaterialEditor", "Parsed Variants :", nullptr));
        label_3->setText(QCoreApplication::translate("MaterialEditor", "Stages :", nullptr));
        RecompileBtn->setText(QCoreApplication::translate("MaterialEditor", "Recompile", nullptr));
        EditBtn->setText(QCoreApplication::translate("MaterialEditor", "Edit Code", nullptr));
        ShowReflectionBtn->setText(QCoreApplication::translate("MaterialEditor", "Show Reflection Info", nullptr));
        label_4->setText(QCoreApplication::translate("MaterialEditor", "Name :", nullptr));
        materialName->setPlaceholderText(QCoreApplication::translate("MaterialEditor", "New Material Name", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MaterialEditor", "Diffuse", nullptr));
        diffuseTexture->setText(QString());
        useDiffuseTexure->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_5->setText(QCoreApplication::translate("MaterialEditor", "Color", nullptr));
        diffuseColor->setText(QString());
        groupBox_6->setTitle(QCoreApplication::translate("MaterialEditor", "Specular", nullptr));
        specTexture->setText(QString());
        useSpecTexture->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_10->setText(QCoreApplication::translate("MaterialEditor", "spec intensity", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MaterialEditor", "Normal", nullptr));
        normalTexture->setText(QString());
        checkBox_3->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MaterialEditor", "Metallic", nullptr));
        metallicTexture->setText(QString());
        checkBox_4->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_8->setText(QCoreApplication::translate("MaterialEditor", "value", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MaterialEditor", "Roughness", nullptr));
        roughnessTexture->setText(QString());
        checkBox_5->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_9->setText(QCoreApplication::translate("MaterialEditor", "value", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MaterialEditor", "Emissive", nullptr));
        emissiveTexture->setText(QString());
        checkBox_7->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_11->setText(QCoreApplication::translate("MaterialEditor", "HDR intensity", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MaterialEditor", "AO", nullptr));
        aoTexture->setText(QString());
        checkBox_8->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_12->setText(QCoreApplication::translate("MaterialEditor", "Color", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MaterialEditor: public Ui_MaterialEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEMATERIALEDITOR_H
