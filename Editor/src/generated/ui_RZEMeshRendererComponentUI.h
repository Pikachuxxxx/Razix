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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeshRendererComponentUI
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QComboBox *comboBox;

    void setupUi(QWidget *MeshRendererComponentUI)
    {
        if (MeshRendererComponentUI->objectName().isEmpty())
            MeshRendererComponentUI->setObjectName(QString::fromUtf8("MeshRendererComponentUI"));
        MeshRendererComponentUI->resize(433, 110);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MeshRendererComponentUI->sizePolicy().hasHeightForWidth());
        MeshRendererComponentUI->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(MeshRendererComponentUI);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        pushButton = new QPushButton(MeshRendererComponentUI);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(20, 20));
        pushButton->setMaximumSize(QSize(20, 20));
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/Browse.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(22, 20));

        gridLayout->addWidget(pushButton, 2, 2, 1, 1);

        checkBox_2 = new QCheckBox(MeshRendererComponentUI);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout->addWidget(checkBox_2, 1, 0, 1, 1);

        checkBox = new QCheckBox(MeshRendererComponentUI);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 0, 0, 1, 1);

        label_2 = new QLabel(MeshRendererComponentUI);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label = new QLabel(MeshRendererComponentUI);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        lineEdit_2 = new QLineEdit(MeshRendererComponentUI);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        gridLayout->addWidget(lineEdit_2, 3, 1, 1, 1);

        pushButton_2 = new QPushButton(MeshRendererComponentUI);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(20, 20));
        pushButton_2->setMaximumSize(QSize(20, 20));
        pushButton_2->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        pushButton_2->setIcon(icon);
        pushButton_2->setIconSize(QSize(20, 20));

        gridLayout->addWidget(pushButton_2, 3, 2, 1, 1);

        comboBox = new QComboBox(MeshRendererComponentUI);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 2, 1, 1, 1);


        retranslateUi(MeshRendererComponentUI);

        QMetaObject::connectSlotsByName(MeshRendererComponentUI);
    } // setupUi

    void retranslateUi(QWidget *MeshRendererComponentUI)
    {
        MeshRendererComponentUI->setWindowTitle(QCoreApplication::translate("MeshRendererComponentUI", "RZEMeshRendererComponentUI", nullptr));
        pushButton->setText(QString());
        checkBox_2->setText(QCoreApplication::translate("MeshRendererComponentUI", "Recieve Shadows", nullptr));
        checkBox->setText(QCoreApplication::translate("MeshRendererComponentUI", "show Bounding Box", nullptr));
        label_2->setText(QCoreApplication::translate("MeshRendererComponentUI", "Material", nullptr));
        label->setText(QCoreApplication::translate("MeshRendererComponentUI", "Mesh", nullptr));
        pushButton_2->setText(QString());
        comboBox->setItemText(0, QCoreApplication::translate("MeshRendererComponentUI", "Plane", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MeshRendererComponentUI", "Screen Quad", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MeshRendererComponentUI", "Cube", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("MeshRendererComponentUI", "Pyramid", nullptr));
        comboBox->setItemText(4, QCoreApplication::translate("MeshRendererComponentUI", "Sphere", nullptr));
        comboBox->setItemText(5, QCoreApplication::translate("MeshRendererComponentUI", "Capsule", nullptr));
        comboBox->setItemText(6, QCoreApplication::translate("MeshRendererComponentUI", "Cylinder", nullptr));

    } // retranslateUi

};

namespace Ui {
    class MeshRendererComponentUI: public Ui_MeshRendererComponentUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEMESHRENDERERCOMPONENTUI_H
