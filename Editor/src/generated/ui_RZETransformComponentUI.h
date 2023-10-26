/********************************************************************************
** Form generated from reading UI file 'RZETransformComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZETRANSFORMCOMPONENTUI_H
#define UI_RZETRANSFORMCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TransformComponent
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_10;
    QLabel *PosLbl;
    QSpacerItem *horizontalSpacer_1;
    QHBoxLayout *horizontalLayout;
    QPushButton *PosX;
    QLineEdit *PosVal_X;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *PosY;
    QLineEdit *PosVal_Y;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *PosZ;
    QLineEdit *PosVal_Z;
    QHBoxLayout *horizontalLayout_11;
    QLabel *RotLbl;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *RotX;
    QLineEdit *RotVal_X;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *RotY;
    QLineEdit *RotVal_Y;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *RotZ;
    QLineEdit *RotVal_Z;
    QHBoxLayout *horizontalLayout_12;
    QLabel *ScaleLbl;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *ScaleX;
    QLineEdit *ScaleVal_X;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *ScaleY;
    QLineEdit *ScaleVal_Y;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *ScaleZ;
    QLineEdit *ScaleVal_Z;

    void setupUi(QWidget *TransformComponent)
    {
        if (TransformComponent->objectName().isEmpty())
            TransformComponent->setObjectName(QString::fromUtf8("TransformComponent"));
        TransformComponent->resize(350, 102);
        TransformComponent->setMinimumSize(QSize(350, 0));
        TransformComponent->setStyleSheet(QString::fromUtf8("border-left: 2px;\n"
"border-right: 2px;\n"
"border-bottom: 2px;\n"
"\n"
""));
        verticalLayout = new QVBoxLayout(TransformComponent);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        PosLbl = new QLabel(TransformComponent);
        PosLbl->setObjectName(QString::fromUtf8("PosLbl"));
        PosLbl->setMinimumSize(QSize(50, 0));
        PosLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_10->addWidget(PosLbl);

        horizontalSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        PosX = new QPushButton(TransformComponent);
        PosX->setObjectName(QString::fromUtf8("PosX"));
        PosX->setMaximumSize(QSize(20, 20));
        PosX->setAutoFillBackground(false);
        PosX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));
        PosX->setFlat(true);

        horizontalLayout->addWidget(PosX);

        PosVal_X = new QLineEdit(TransformComponent);
        PosVal_X->setObjectName(QString::fromUtf8("PosVal_X"));
        PosVal_X->setMinimumSize(QSize(60, 0));
        PosVal_X->setMaximumSize(QSize(80, 20));
        PosVal_X->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));
        PosVal_X->setDragEnabled(true);

        horizontalLayout->addWidget(PosVal_X);


        horizontalLayout_10->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        PosY = new QPushButton(TransformComponent);
        PosY->setObjectName(QString::fromUtf8("PosY"));
        PosY->setMaximumSize(QSize(20, 20));
        PosY->setAutoFillBackground(false);
        PosY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));
        PosY->setFlat(true);

        horizontalLayout_4->addWidget(PosY);

        PosVal_Y = new QLineEdit(TransformComponent);
        PosVal_Y->setObjectName(QString::fromUtf8("PosVal_Y"));
        PosVal_Y->setMinimumSize(QSize(60, 0));
        PosVal_Y->setMaximumSize(QSize(80, 20));
        PosVal_Y->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_4->addWidget(PosVal_Y);


        horizontalLayout_10->addLayout(horizontalLayout_4);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        PosZ = new QPushButton(TransformComponent);
        PosZ->setObjectName(QString::fromUtf8("PosZ"));
        PosZ->setMaximumSize(QSize(20, 20));
        PosZ->setAutoFillBackground(false);
        PosZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));
        PosZ->setFlat(true);

        horizontalLayout_7->addWidget(PosZ);

        PosVal_Z = new QLineEdit(TransformComponent);
        PosVal_Z->setObjectName(QString::fromUtf8("PosVal_Z"));
        PosVal_Z->setMinimumSize(QSize(60, 0));
        PosVal_Z->setMaximumSize(QSize(80, 20));
        PosVal_Z->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 150, 225);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_7->addWidget(PosVal_Z);


        horizontalLayout_10->addLayout(horizontalLayout_7);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        RotLbl = new QLabel(TransformComponent);
        RotLbl->setObjectName(QString::fromUtf8("RotLbl"));
        RotLbl->setMinimumSize(QSize(50, 0));
        RotLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_11->addWidget(RotLbl);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        RotX = new QPushButton(TransformComponent);
        RotX->setObjectName(QString::fromUtf8("RotX"));
        RotX->setMaximumSize(QSize(20, 20));
        RotX->setAutoFillBackground(false);
        RotX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));
        RotX->setFlat(true);

        horizontalLayout_2->addWidget(RotX);

        RotVal_X = new QLineEdit(TransformComponent);
        RotVal_X->setObjectName(QString::fromUtf8("RotVal_X"));
        RotVal_X->setMinimumSize(QSize(60, 0));
        RotVal_X->setMaximumSize(QSize(80, 20));
        RotVal_X->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));
        RotVal_X->setDragEnabled(true);

        horizontalLayout_2->addWidget(RotVal_X);


        horizontalLayout_11->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        RotY = new QPushButton(TransformComponent);
        RotY->setObjectName(QString::fromUtf8("RotY"));
        RotY->setMaximumSize(QSize(20, 20));
        RotY->setAutoFillBackground(false);
        RotY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));
        RotY->setFlat(true);

        horizontalLayout_5->addWidget(RotY);

        RotVal_Y = new QLineEdit(TransformComponent);
        RotVal_Y->setObjectName(QString::fromUtf8("RotVal_Y"));
        RotVal_Y->setMinimumSize(QSize(60, 0));
        RotVal_Y->setMaximumSize(QSize(80, 20));
        RotVal_Y->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_5->addWidget(RotVal_Y);


        horizontalLayout_11->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        RotZ = new QPushButton(TransformComponent);
        RotZ->setObjectName(QString::fromUtf8("RotZ"));
        RotZ->setMaximumSize(QSize(20, 20));
        RotZ->setAutoFillBackground(false);
        RotZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));
        RotZ->setFlat(true);

        horizontalLayout_8->addWidget(RotZ);

        RotVal_Z = new QLineEdit(TransformComponent);
        RotVal_Z->setObjectName(QString::fromUtf8("RotVal_Z"));
        RotVal_Z->setMinimumSize(QSize(60, 0));
        RotVal_Z->setMaximumSize(QSize(80, 20));
        RotVal_Z->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 150, 225);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_8->addWidget(RotVal_Z);


        horizontalLayout_11->addLayout(horizontalLayout_8);


        verticalLayout->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        ScaleLbl = new QLabel(TransformComponent);
        ScaleLbl->setObjectName(QString::fromUtf8("ScaleLbl"));
        ScaleLbl->setMinimumSize(QSize(50, 0));
        ScaleLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_12->addWidget(ScaleLbl);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        ScaleX = new QPushButton(TransformComponent);
        ScaleX->setObjectName(QString::fromUtf8("ScaleX"));
        ScaleX->setMaximumSize(QSize(20, 20));
        ScaleX->setAutoFillBackground(false);
        ScaleX->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));
        ScaleX->setFlat(true);

        horizontalLayout_3->addWidget(ScaleX);

        ScaleVal_X = new QLineEdit(TransformComponent);
        ScaleVal_X->setObjectName(QString::fromUtf8("ScaleVal_X"));
        ScaleVal_X->setMinimumSize(QSize(60, 0));
        ScaleVal_X->setMaximumSize(QSize(80, 20));
        ScaleVal_X->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));
        ScaleVal_X->setDragEnabled(true);

        horizontalLayout_3->addWidget(ScaleVal_X);


        horizontalLayout_12->addLayout(horizontalLayout_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        ScaleY = new QPushButton(TransformComponent);
        ScaleY->setObjectName(QString::fromUtf8("ScaleY"));
        ScaleY->setMaximumSize(QSize(20, 20));
        ScaleY->setAutoFillBackground(false);
        ScaleY->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));
        ScaleY->setFlat(true);

        horizontalLayout_6->addWidget(ScaleY);

        ScaleVal_Y = new QLineEdit(TransformComponent);
        ScaleVal_Y->setObjectName(QString::fromUtf8("ScaleVal_Y"));
        ScaleVal_Y->setMinimumSize(QSize(60, 0));
        ScaleVal_Y->setMaximumSize(QSize(80, 20));
        ScaleVal_Y->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_6->addWidget(ScaleVal_Y);


        horizontalLayout_12->addLayout(horizontalLayout_6);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        ScaleZ = new QPushButton(TransformComponent);
        ScaleZ->setObjectName(QString::fromUtf8("ScaleZ"));
        ScaleZ->setMaximumSize(QSize(20, 20));
        ScaleZ->setAutoFillBackground(false);
        ScaleZ->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));
        ScaleZ->setFlat(true);

        horizontalLayout_9->addWidget(ScaleZ);

        ScaleVal_Z = new QLineEdit(TransformComponent);
        ScaleVal_Z->setObjectName(QString::fromUtf8("ScaleVal_Z"));
        ScaleVal_Z->setMinimumSize(QSize(60, 0));
        ScaleVal_Z->setMaximumSize(QSize(80, 20));
        ScaleVal_Z->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 150, 225);\n"
"border-width:2px;\n"
"border-style:solid;\n"
"border-radius:0px;"));

        horizontalLayout_9->addWidget(ScaleVal_Z);


        horizontalLayout_12->addLayout(horizontalLayout_9);


        verticalLayout->addLayout(horizontalLayout_12);


        retranslateUi(TransformComponent);

        QMetaObject::connectSlotsByName(TransformComponent);
    } // setupUi

    void retranslateUi(QWidget *TransformComponent)
    {
        TransformComponent->setWindowTitle(QCoreApplication::translate("TransformComponent", "Form", nullptr));
        PosLbl->setText(QCoreApplication::translate("TransformComponent", "Position", nullptr));
        PosX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        PosVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        PosY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        PosVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        PosZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        PosVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotLbl->setText(QCoreApplication::translate("TransformComponent", "Rotation", nullptr));
        RotX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        RotVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        RotVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        RotZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        RotVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleLbl->setText(QCoreApplication::translate("TransformComponent", "Scale", nullptr));
        ScaleX->setText(QCoreApplication::translate("TransformComponent", "x", nullptr));
        ScaleVal_X->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleY->setText(QCoreApplication::translate("TransformComponent", "y", nullptr));
        ScaleVal_Y->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
        ScaleZ->setText(QCoreApplication::translate("TransformComponent", "z", nullptr));
        ScaleVal_Z->setText(QCoreApplication::translate("TransformComponent", "0.000000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TransformComponent: public Ui_TransformComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZETRANSFORMCOMPONENTUI_H
