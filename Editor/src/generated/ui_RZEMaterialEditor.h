/********************************************************************************
** Form generated from reading UI file 'RZEMaterialEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEMATERIALEDITOR_H
#define UI_RZEMATERIALEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MaterialEditor
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *materialName;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_21;
    QLabel *label;
    QLabel *shaderNameLbl;
    QLabel *label_2;
    QLabel *parseVariantsLbl;
    QLabel *label_3;
    QLabel *stagesLbl;
    QPushButton *RecompileBtn;
    QHBoxLayout *horizontalLayout;
    QPushButton *EditBtn;
    QPushButton *ShowReflectionBtn;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_21;
    QHBoxLayout *horizontalLayout_16;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_18;
    QLineEdit *UVScaleX;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_19;
    QLineEdit *UVScaleY;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_7;
    QComboBox *workflow;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *diffuseTexture;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *useDiffuseTexure;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QPushButton *diffuseColor;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *normalTexture;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_6;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_9;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *emissiveTexture;
    QVBoxLayout *verticalLayout_8;
    QCheckBox *checkBox_7;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_11;
    QLineEdit *emissiveIntensity;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *specTexture;
    QVBoxLayout *verticalLayout_7;
    QCheckBox *useSpecTexture;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_10;
    QLineEdit *specIntensity;
    QSpacerItem *horizontalSpacer_18;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_13;
    QLabel *label_17;
    QVBoxLayout *verticalLayout_14;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *metalRoughnessAOMap;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *metallicTexture;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_15;
    QLineEdit *metallicValue;
    QSpacerItem *horizontalSpacer_19;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *roughnessTexture;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_16;
    QLineEdit *roughnessValue;
    QSpacerItem *horizontalSpacer_20;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *aoTexture;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_20;
    QLineEdit *aoIntensity;
    QSpacerItem *horizontalSpacer_17;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *MaterialEditor)
    {
        if (MaterialEditor->objectName().isEmpty())
            MaterialEditor->setObjectName(QString::fromUtf8("MaterialEditor"));
        MaterialEditor->resize(447, 867);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/RazixMaterial.png"), QSize(), QIcon::Normal, QIcon::Off);
        MaterialEditor->setWindowIcon(icon);
        MaterialEditor->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(MaterialEditor);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4 = new QLabel(MaterialEditor);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setMaximumSize(QSize(16777215, 50));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        materialName = new QLineEdit(MaterialEditor);
        materialName->setObjectName(QString::fromUtf8("materialName"));
        materialName->setMaximumSize(QSize(16777215, 50));
        materialName->setFrame(true);

        horizontalLayout_7->addWidget(materialName);

        pushButton = new QPushButton(MaterialEditor);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_7->addWidget(pushButton);

        pushButton_2 = new QPushButton(MaterialEditor);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/Browse.png);"));
        pushButton_2->setFlat(true);

        horizontalLayout_7->addWidget(pushButton_2);

        horizontalSpacer_21 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_21);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout_7);

        label = new QLabel(MaterialEditor);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        shaderNameLbl = new QLabel(MaterialEditor);
        shaderNameLbl->setObjectName(QString::fromUtf8("shaderNameLbl"));
        shaderNameLbl->setStyleSheet(QString::fromUtf8("border-color: rgb(147, 147, 147);\n"
"border:1px;\n"
"border-radius:5;\n"
"border-style:solid;"));

        formLayout->setWidget(1, QFormLayout::FieldRole, shaderNameLbl);

        label_2 = new QLabel(MaterialEditor);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        parseVariantsLbl = new QLabel(MaterialEditor);
        parseVariantsLbl->setObjectName(QString::fromUtf8("parseVariantsLbl"));

        formLayout->setWidget(2, QFormLayout::FieldRole, parseVariantsLbl);

        label_3 = new QLabel(MaterialEditor);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        stagesLbl = new QLabel(MaterialEditor);
        stagesLbl->setObjectName(QString::fromUtf8("stagesLbl"));

        formLayout->setWidget(3, QFormLayout::FieldRole, stagesLbl);

        RecompileBtn = new QPushButton(MaterialEditor);
        RecompileBtn->setObjectName(QString::fromUtf8("RecompileBtn"));

        formLayout->setWidget(4, QFormLayout::LabelRole, RecompileBtn);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        EditBtn = new QPushButton(MaterialEditor);
        EditBtn->setObjectName(QString::fromUtf8("EditBtn"));

        horizontalLayout->addWidget(EditBtn);

        ShowReflectionBtn = new QPushButton(MaterialEditor);
        ShowReflectionBtn->setObjectName(QString::fromUtf8("ShowReflectionBtn"));

        horizontalLayout->addWidget(ShowReflectionBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        formLayout->setLayout(4, QFormLayout::FieldRole, horizontalLayout);

        label_21 = new QLabel(MaterialEditor);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_21);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(0);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_18 = new QLabel(MaterialEditor);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setMaximumSize(QSize(20, 16777215));
        label_18->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"background-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;"));
        label_18->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(label_18);

        UVScaleX = new QLineEdit(MaterialEditor);
        UVScaleX->setObjectName(QString::fromUtf8("UVScaleX"));
        UVScaleX->setMaximumSize(QSize(40, 16777215));
        UVScaleX->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;"));

        horizontalLayout_12->addWidget(UVScaleX);


        horizontalLayout_16->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(0);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_19 = new QLabel(MaterialEditor);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setMaximumSize(QSize(20, 16777215));
        label_19->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"background-color: rgb(0, 255, 0);"));
        label_19->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(label_19);

        UVScaleY = new QLineEdit(MaterialEditor);
        UVScaleY->setObjectName(QString::fromUtf8("UVScaleY"));
        UVScaleY->setMaximumSize(QSize(40, 16777215));
        UVScaleY->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;"));

        horizontalLayout_13->addWidget(UVScaleY);


        horizontalLayout_16->addLayout(horizontalLayout_13);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_11);


        formLayout->setLayout(5, QFormLayout::FieldRole, horizontalLayout_16);

        label_7 = new QLabel(MaterialEditor);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        QFont font;
        font.setPointSize(12);
        label_7->setFont(font);

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        workflow = new QComboBox(MaterialEditor);
        workflow->addItem(QString());
        workflow->addItem(QString());
        workflow->addItem(QString());
        workflow->addItem(QString());
        workflow->addItem(QString());
        workflow->addItem(QString());
        workflow->setObjectName(QString::fromUtf8("workflow"));
        workflow->setMaximumSize(QSize(250, 16777215));
        QFont font1;
        font1.setPointSize(8);
        workflow->setFont(font1);

        formLayout->setWidget(6, QFormLayout::FieldRole, workflow);

        label_6 = new QLabel(MaterialEditor);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        diffuseTexture = new QPushButton(MaterialEditor);
        diffuseTexture->setObjectName(QString::fromUtf8("diffuseTexture"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(diffuseTexture->sizePolicy().hasHeightForWidth());
        diffuseTexture->setSizePolicy(sizePolicy1);
        diffuseTexture->setMinimumSize(QSize(50, 50));
        diffuseTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));
        diffuseTexture->setIconSize(QSize(50, 50));
        diffuseTexture->setFlat(false);

        horizontalLayout_2->addWidget(diffuseTexture);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        useDiffuseTexure = new QCheckBox(MaterialEditor);
        useDiffuseTexure->setObjectName(QString::fromUtf8("useDiffuseTexure"));

        verticalLayout_2->addWidget(useDiffuseTexure);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(MaterialEditor);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(75, 0));
        label_5->setMaximumSize(QSize(75, 16777215));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_5);

        diffuseColor = new QPushButton(MaterialEditor);
        diffuseColor->setObjectName(QString::fromUtf8("diffuseColor"));
        diffuseColor->setMinimumSize(QSize(100, 0));
        diffuseColor->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_4->addWidget(diffuseColor);


        verticalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        formLayout->setLayout(7, QFormLayout::FieldRole, horizontalLayout_2);

        label_8 = new QLabel(MaterialEditor);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMinimumSize(QSize(75, 0));
        label_8->setMaximumSize(QSize(16777215, 16777215));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(8, QFormLayout::LabelRole, label_8);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        normalTexture = new QPushButton(MaterialEditor);
        normalTexture->setObjectName(QString::fromUtf8("normalTexture"));
        sizePolicy1.setHeightForWidth(normalTexture->sizePolicy().hasHeightForWidth());
        normalTexture->setSizePolicy(sizePolicy1);
        normalTexture->setMinimumSize(QSize(50, 50));
        normalTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_5->addWidget(normalTexture);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        checkBox_3 = new QCheckBox(MaterialEditor);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setMinimumSize(QSize(0, 0));

        verticalLayout_5->addWidget(checkBox_3);

        checkBox_6 = new QCheckBox(MaterialEditor);
        checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));
        checkBox_6->setMinimumSize(QSize(0, 0));

        verticalLayout_5->addWidget(checkBox_6);


        horizontalLayout_5->addLayout(verticalLayout_5);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);


        formLayout->setLayout(8, QFormLayout::FieldRole, horizontalLayout_5);

        label_9 = new QLabel(MaterialEditor);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setMinimumSize(QSize(75, 0));
        label_9->setMaximumSize(QSize(16777215, 16777215));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(10, QFormLayout::LabelRole, label_9);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        emissiveTexture = new QPushButton(MaterialEditor);
        emissiveTexture->setObjectName(QString::fromUtf8("emissiveTexture"));
        sizePolicy1.setHeightForWidth(emissiveTexture->sizePolicy().hasHeightForWidth());
        emissiveTexture->setSizePolicy(sizePolicy1);
        emissiveTexture->setMinimumSize(QSize(50, 50));
        emissiveTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_6->addWidget(emissiveTexture);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        checkBox_7 = new QCheckBox(MaterialEditor);
        checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));

        verticalLayout_8->addWidget(checkBox_7);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_11 = new QLabel(MaterialEditor);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMinimumSize(QSize(75, 0));
        label_11->setMaximumSize(QSize(75, 16777215));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_17->addWidget(label_11);

        emissiveIntensity = new QLineEdit(MaterialEditor);
        emissiveIntensity->setObjectName(QString::fromUtf8("emissiveIntensity"));
        emissiveIntensity->setMinimumSize(QSize(100, 0));
        emissiveIntensity->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_17->addWidget(emissiveIntensity);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_7);


        verticalLayout_8->addLayout(horizontalLayout_17);


        horizontalLayout_6->addLayout(verticalLayout_8);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        formLayout->setLayout(10, QFormLayout::FieldRole, horizontalLayout_6);

        label_12 = new QLabel(MaterialEditor);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setMinimumSize(QSize(75, 0));
        label_12->setMaximumSize(QSize(16777215, 16777215));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(11, QFormLayout::LabelRole, label_12);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        specTexture = new QPushButton(MaterialEditor);
        specTexture->setObjectName(QString::fromUtf8("specTexture"));
        sizePolicy1.setHeightForWidth(specTexture->sizePolicy().hasHeightForWidth());
        specTexture->setSizePolicy(sizePolicy1);
        specTexture->setMinimumSize(QSize(50, 50));
        specTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_3->addWidget(specTexture);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        useSpecTexture = new QCheckBox(MaterialEditor);
        useSpecTexture->setObjectName(QString::fromUtf8("useSpecTexture"));

        verticalLayout_7->addWidget(useSpecTexture);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_10 = new QLabel(MaterialEditor);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMinimumSize(QSize(75, 0));
        label_10->setMaximumSize(QSize(75, 16777215));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_15->addWidget(label_10);

        specIntensity = new QLineEdit(MaterialEditor);
        specIntensity->setObjectName(QString::fromUtf8("specIntensity"));
        specIntensity->setMinimumSize(QSize(100, 0));
        specIntensity->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_15->addWidget(specIntensity);

        horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_18);


        verticalLayout_7->addLayout(horizontalLayout_15);


        horizontalLayout_3->addLayout(verticalLayout_7);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        formLayout->setLayout(11, QFormLayout::FieldRole, horizontalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_13 = new QLabel(MaterialEditor);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setMinimumSize(QSize(75, 0));
        label_13->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_4->addWidget(label_13);

        label_17 = new QLabel(MaterialEditor);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        QFont font2;
        font2.setItalic(false);
        label_17->setFont(font2);

        verticalLayout_4->addWidget(label_17);


        formLayout->setLayout(12, QFormLayout::LabelRole, verticalLayout_4);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(6);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        metalRoughnessAOMap = new QPushButton(MaterialEditor);
        metalRoughnessAOMap->setObjectName(QString::fromUtf8("metalRoughnessAOMap"));
        sizePolicy1.setHeightForWidth(metalRoughnessAOMap->sizePolicy().hasHeightForWidth());
        metalRoughnessAOMap->setSizePolicy(sizePolicy1);
        metalRoughnessAOMap->setMinimumSize(QSize(50, 50));
        metalRoughnessAOMap->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_8->addWidget(metalRoughnessAOMap);

        label_14 = new QLabel(MaterialEditor);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_8->addWidget(label_14);


        verticalLayout_14->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        metallicTexture = new QPushButton(MaterialEditor);
        metallicTexture->setObjectName(QString::fromUtf8("metallicTexture"));
        sizePolicy1.setHeightForWidth(metallicTexture->sizePolicy().hasHeightForWidth());
        metallicTexture->setSizePolicy(sizePolicy1);
        metallicTexture->setMinimumSize(QSize(50, 50));
        metallicTexture->setAutoFillBackground(false);
        metallicTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_9->addWidget(metallicTexture);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_15 = new QLabel(MaterialEditor);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setMinimumSize(QSize(75, 0));
        label_15->setMaximumSize(QSize(75, 16777215));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_14->addWidget(label_15);

        metallicValue = new QLineEdit(MaterialEditor);
        metallicValue->setObjectName(QString::fromUtf8("metallicValue"));
        metallicValue->setMinimumSize(QSize(100, 0));
        metallicValue->setMaximumSize(QSize(100, 16777215));
        metallicValue->setAutoFillBackground(false);
        metallicValue->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

        horizontalLayout_14->addWidget(metallicValue);

        horizontalSpacer_19 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_19);


        horizontalLayout_9->addLayout(horizontalLayout_14);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_4);


        verticalLayout_14->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        roughnessTexture = new QPushButton(MaterialEditor);
        roughnessTexture->setObjectName(QString::fromUtf8("roughnessTexture"));
        sizePolicy1.setHeightForWidth(roughnessTexture->sizePolicy().hasHeightForWidth());
        roughnessTexture->setSizePolicy(sizePolicy1);
        roughnessTexture->setMinimumSize(QSize(50, 50));
        roughnessTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_10->addWidget(roughnessTexture);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setSpacing(6);
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        label_16 = new QLabel(MaterialEditor);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setMinimumSize(QSize(75, 0));
        label_16->setMaximumSize(QSize(75, 16777215));
        label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_20->addWidget(label_16);

        roughnessValue = new QLineEdit(MaterialEditor);
        roughnessValue->setObjectName(QString::fromUtf8("roughnessValue"));
        roughnessValue->setMinimumSize(QSize(100, 0));
        roughnessValue->setMaximumSize(QSize(100, 16777215));
        roughnessValue->setAutoFillBackground(false);
        roughnessValue->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

        horizontalLayout_20->addWidget(roughnessValue);

        horizontalSpacer_20 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_20);


        horizontalLayout_10->addLayout(horizontalLayout_20);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_9);


        verticalLayout_14->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        aoTexture = new QPushButton(MaterialEditor);
        aoTexture->setObjectName(QString::fromUtf8("aoTexture"));
        sizePolicy1.setHeightForWidth(aoTexture->sizePolicy().hasHeightForWidth());
        aoTexture->setSizePolicy(sizePolicy1);
        aoTexture->setMinimumSize(QSize(50, 50));
        aoTexture->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/select_texture_placeholder.png);"));

        horizontalLayout_11->addWidget(aoTexture);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setSpacing(6);
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        label_20 = new QLabel(MaterialEditor);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setMinimumSize(QSize(75, 0));
        label_20->setMaximumSize(QSize(75, 16777215));
        label_20->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_21->addWidget(label_20);

        aoIntensity = new QLineEdit(MaterialEditor);
        aoIntensity->setObjectName(QString::fromUtf8("aoIntensity"));
        aoIntensity->setMinimumSize(QSize(100, 0));
        aoIntensity->setMaximumSize(QSize(100, 16777215));
        aoIntensity->setAutoFillBackground(false);
        aoIntensity->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

        horizontalLayout_21->addWidget(aoIntensity);

        horizontalSpacer_17 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_17);


        horizontalLayout_11->addLayout(horizontalLayout_21);


        verticalLayout_14->addLayout(horizontalLayout_11);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_14->addItem(verticalSpacer_3);


        formLayout->setLayout(12, QFormLayout::FieldRole, verticalLayout_14);


        verticalLayout->addLayout(formLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        retranslateUi(MaterialEditor);

        QMetaObject::connectSlotsByName(MaterialEditor);
    } // setupUi

    void retranslateUi(QWidget *MaterialEditor)
    {
        MaterialEditor->setWindowTitle(QCoreApplication::translate("MaterialEditor", "Material Editor", nullptr));
        label_4->setText(QCoreApplication::translate("MaterialEditor", "Name :", nullptr));
        materialName->setPlaceholderText(QCoreApplication::translate("MaterialEditor", "Enter New Material Name", nullptr));
        pushButton->setText(QCoreApplication::translate("MaterialEditor", "Save", nullptr));
        pushButton_2->setText(QString());
        label->setText(QCoreApplication::translate("MaterialEditor", "Shader :", nullptr));
        shaderNameLbl->setText(QCoreApplication::translate("MaterialEditor", "Shader.Builtin.PBRIBL.rzsf", nullptr));
        label_2->setText(QCoreApplication::translate("MaterialEditor", "Parsed Variants :", nullptr));
        parseVariantsLbl->setText(QCoreApplication::translate("MaterialEditor", "LIT, UNLIT, WIREFRAME", nullptr));
        label_3->setText(QCoreApplication::translate("MaterialEditor", "Stages :", nullptr));
        stagesLbl->setText(QCoreApplication::translate("MaterialEditor", "VS PS", nullptr));
        RecompileBtn->setText(QCoreApplication::translate("MaterialEditor", "Recompile", nullptr));
        EditBtn->setText(QCoreApplication::translate("MaterialEditor", "Edit Code", nullptr));
        ShowReflectionBtn->setText(QCoreApplication::translate("MaterialEditor", "Show Reflection Info", nullptr));
        label_21->setText(QCoreApplication::translate("MaterialEditor", "UV Scale", nullptr));
        label_18->setText(QCoreApplication::translate("MaterialEditor", "U", nullptr));
        UVScaleX->setText(QCoreApplication::translate("MaterialEditor", "1", nullptr));
        label_19->setText(QCoreApplication::translate("MaterialEditor", "V", nullptr));
        UVScaleY->setText(QCoreApplication::translate("MaterialEditor", "1", nullptr));
        label_7->setText(QCoreApplication::translate("MaterialEditor", "Material Textures (PBR)", nullptr));
        workflow->setItemText(0, QCoreApplication::translate("MaterialEditor", "WORLFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED (BGR)", nullptr));
        workflow->setItemText(1, QCoreApplication::translate("MaterialEditor", "WORLFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE", nullptr));
        workflow->setItemText(2, QCoreApplication::translate("MaterialEditor", "WORLFLOW_PBR_SPECULAR_GLOSS_COMBINED", nullptr));
        workflow->setItemText(3, QCoreApplication::translate("MaterialEditor", "WORLFLOW_PBR_SPECULAR_GLOSS_SEPARATE", nullptr));
        workflow->setItemText(4, QCoreApplication::translate("MaterialEditor", "WORKFLOW_UNLIT", nullptr));
        workflow->setItemText(5, QCoreApplication::translate("MaterialEditor", "WORLFLOW_LIT_PHONG", nullptr));

        label_6->setText(QCoreApplication::translate("MaterialEditor", "Albedo Map", nullptr));
        diffuseTexture->setText(QString());
        useDiffuseTexure->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_5->setText(QCoreApplication::translate("MaterialEditor", "Color", nullptr));
        diffuseColor->setText(QString());
        label_8->setText(QCoreApplication::translate("MaterialEditor", "Normal Map", nullptr));
        normalTexture->setText(QString());
        checkBox_3->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        checkBox_6->setText(QCoreApplication::translate("MaterialEditor", "Invert Normals", nullptr));
        label_9->setText(QCoreApplication::translate("MaterialEditor", "Emissive Map", nullptr));
        emissiveTexture->setText(QString());
        checkBox_7->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_11->setText(QCoreApplication::translate("MaterialEditor", "HDR intensity", nullptr));
        label_12->setText(QCoreApplication::translate("MaterialEditor", "Specular Map", nullptr));
        specTexture->setText(QString());
        useSpecTexture->setText(QCoreApplication::translate("MaterialEditor", "Use Texture", nullptr));
        label_10->setText(QCoreApplication::translate("MaterialEditor", "Spec intensity", nullptr));
        label_13->setText(QCoreApplication::translate("MaterialEditor", "MetallicRoughnessAO Map", nullptr));
        label_17->setText(QCoreApplication::translate("MaterialEditor", "(Order is BGR respectively)", nullptr));
        metalRoughnessAOMap->setText(QString());
        label_14->setText(QCoreApplication::translate("MaterialEditor", "Combined AORoughnessMetallicMap", nullptr));
        metallicTexture->setText(QString());
        label_15->setText(QCoreApplication::translate("MaterialEditor", "Metallic", nullptr));
        roughnessTexture->setText(QString());
        label_16->setText(QCoreApplication::translate("MaterialEditor", "Roughness", nullptr));
        aoTexture->setText(QString());
        label_20->setText(QCoreApplication::translate("MaterialEditor", "AO intensity", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MaterialEditor: public Ui_MaterialEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEMATERIALEDITOR_H
