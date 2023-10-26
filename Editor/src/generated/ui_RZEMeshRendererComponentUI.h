/********************************************************************************
** Form generated from reading UI file 'RZEMeshRendererComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEMESHRENDERERCOMPONENTUI_H
#define UI_RZEMESHRENDERERCOMPONENTUI_H

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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeshRendererComponentUI
{
public:
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;
    QLabel *label_3;
    QCheckBox *receiveShadows;
    QLabel *label_4;
    QCheckBox *showBoundingBox;

    void setupUi(QWidget *MeshRendererComponentUI)
    {
        if (MeshRendererComponentUI->objectName().isEmpty())
            MeshRendererComponentUI->setObjectName(QString::fromUtf8("MeshRendererComponentUI"));
        MeshRendererComponentUI->resize(349, 148);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MeshRendererComponentUI->sizePolicy().hasHeightForWidth());
        MeshRendererComponentUI->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(MeshRendererComponentUI);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        comboBox = new QComboBox(MeshRendererComponentUI);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout_2->addWidget(comboBox);

        pushButton = new QPushButton(MeshRendererComponentUI);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(20, 20));
        pushButton->setMaximumSize(QSize(20, 20));
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/Browse.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(22, 20));

        horizontalLayout_2->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout_2);

        label_2 = new QLabel(MeshRendererComponentUI);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit_2 = new QLineEdit(MeshRendererComponentUI);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        horizontalLayout->addWidget(lineEdit_2);

        pushButton_2 = new QPushButton(MeshRendererComponentUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(20, 20));
        pushButton_2->setMaximumSize(QSize(20, 20));
        pushButton_2->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        pushButton_2->setIcon(icon);
        pushButton_2->setIconSize(QSize(20, 20));

        horizontalLayout->addWidget(pushButton_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout);

        label = new QLabel(MeshRendererComponentUI);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_3 = new QLabel(MeshRendererComponentUI);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        receiveShadows = new QCheckBox(MeshRendererComponentUI);
        receiveShadows->setObjectName(QString::fromUtf8("receiveShadows"));
        receiveShadows->setChecked(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, receiveShadows);

        label_4 = new QLabel(MeshRendererComponentUI);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        showBoundingBox = new QCheckBox(MeshRendererComponentUI);
        showBoundingBox->setObjectName(QString::fromUtf8("showBoundingBox"));
        showBoundingBox->setChecked(false);

        formLayout->setWidget(3, QFormLayout::FieldRole, showBoundingBox);


        retranslateUi(MeshRendererComponentUI);

        QMetaObject::connectSlotsByName(MeshRendererComponentUI);
    } // setupUi

    void retranslateUi(QWidget *MeshRendererComponentUI)
    {
        MeshRendererComponentUI->setWindowTitle(QCoreApplication::translate("MeshRendererComponentUI", "RZEMeshRendererComponentUI", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MeshRendererComponentUI", "Plane", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MeshRendererComponentUI", "Screen Quad", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MeshRendererComponentUI", "Cube", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("MeshRendererComponentUI", "Pyramid", nullptr));
        comboBox->setItemText(4, QCoreApplication::translate("MeshRendererComponentUI", "Sphere", nullptr));
        comboBox->setItemText(5, QCoreApplication::translate("MeshRendererComponentUI", "Capsule", nullptr));
        comboBox->setItemText(6, QCoreApplication::translate("MeshRendererComponentUI", "Cylinder", nullptr));

        pushButton->setText(QString());
        label_2->setText(QCoreApplication::translate("MeshRendererComponentUI", "Material", nullptr));
        pushButton_2->setText(QString());
        label->setText(QCoreApplication::translate("MeshRendererComponentUI", "Mesh", nullptr));
        label_3->setText(QCoreApplication::translate("MeshRendererComponentUI", "Recieve Shadows", nullptr));
        receiveShadows->setText(QString());
        label_4->setText(QCoreApplication::translate("MeshRendererComponentUI", "Show Bounding Box", nullptr));
        showBoundingBox->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MeshRendererComponentUI: public Ui_MeshRendererComponentUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEMESHRENDERERCOMPONENTUI_H
