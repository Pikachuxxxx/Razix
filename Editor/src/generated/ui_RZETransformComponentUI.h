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
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TransformComponent
{
public:
    QVBoxLayout *verticalLayout;
    QToolBox *toolBox;
    QWidget *Transform;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *Position_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton_7;
    QLineEdit *Position_Value_3;
    QPushButton *pushButton_8;
    QLineEdit *lineEdit_5;
    QPushButton *pushButton_9;
    QLineEdit *lineEdit_6;
    QHBoxLayout *horizontalLayout;
    QLabel *Position;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QLineEdit *Position_Value;
    QPushButton *pushButton_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton_3;
    QLineEdit *lineEdit_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *Position_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_4;
    QLineEdit *Position_Value_2;
    QPushButton *pushButton_5;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton_6;
    QLineEdit *lineEdit_4;

    void setupUi(QWidget *TransformComponent)
    {
        if (TransformComponent->objectName().isEmpty())
            TransformComponent->setObjectName(QString::fromUtf8("TransformComponent"));
        TransformComponent->resize(405, 171);
        verticalLayout = new QVBoxLayout(TransformComponent);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolBox = new QToolBox(TransformComponent);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        Transform = new QWidget();
        Transform->setObjectName(QString::fromUtf8("Transform"));
        Transform->setGeometry(QRect(0, 0, 387, 123));
        verticalLayout_2 = new QVBoxLayout(Transform);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        Position_3 = new QLabel(Transform);
        Position_3->setObjectName(QString::fromUtf8("Position_3"));

        horizontalLayout_3->addWidget(Position_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        pushButton_7 = new QPushButton(Transform);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setMaximumSize(QSize(30, 30));
        pushButton_7->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        horizontalLayout_3->addWidget(pushButton_7);

        Position_Value_3 = new QLineEdit(Transform);
        Position_Value_3->setObjectName(QString::fromUtf8("Position_Value_3"));
        Position_Value_3->setMaximumSize(QSize(40, 30));
        Position_Value_3->setDragEnabled(true);

        horizontalLayout_3->addWidget(Position_Value_3);

        pushButton_8 = new QPushButton(Transform);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setMaximumSize(QSize(30, 30));
        pushButton_8->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        horizontalLayout_3->addWidget(pushButton_8);

        lineEdit_5 = new QLineEdit(Transform);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));
        lineEdit_5->setMaximumSize(QSize(40, 30));

        horizontalLayout_3->addWidget(lineEdit_5);

        pushButton_9 = new QPushButton(Transform);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setMaximumSize(QSize(30, 30));
        pushButton_9->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        horizontalLayout_3->addWidget(pushButton_9);

        lineEdit_6 = new QLineEdit(Transform);
        lineEdit_6->setObjectName(QString::fromUtf8("lineEdit_6"));
        lineEdit_6->setMaximumSize(QSize(40, 30));

        horizontalLayout_3->addWidget(lineEdit_6);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Position = new QLabel(Transform);
        Position->setObjectName(QString::fromUtf8("Position"));

        horizontalLayout->addWidget(Position);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(Transform);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(30, 30));
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        horizontalLayout->addWidget(pushButton);

        Position_Value = new QLineEdit(Transform);
        Position_Value->setObjectName(QString::fromUtf8("Position_Value"));
        Position_Value->setMaximumSize(QSize(40, 30));
        Position_Value->setDragEnabled(true);

        horizontalLayout->addWidget(Position_Value);

        pushButton_2 = new QPushButton(Transform);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setMaximumSize(QSize(30, 30));
        pushButton_2->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        horizontalLayout->addWidget(pushButton_2);

        lineEdit = new QLineEdit(Transform);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMaximumSize(QSize(40, 30));

        horizontalLayout->addWidget(lineEdit);

        pushButton_3 = new QPushButton(Transform);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setMaximumSize(QSize(30, 30));
        pushButton_3->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        horizontalLayout->addWidget(pushButton_3);

        lineEdit_2 = new QLineEdit(Transform);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setMaximumSize(QSize(40, 30));

        horizontalLayout->addWidget(lineEdit_2);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Position_2 = new QLabel(Transform);
        Position_2->setObjectName(QString::fromUtf8("Position_2"));

        horizontalLayout_2->addWidget(Position_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pushButton_4 = new QPushButton(Transform);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setMaximumSize(QSize(30, 30));
        pushButton_4->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 4);"));

        horizontalLayout_2->addWidget(pushButton_4);

        Position_Value_2 = new QLineEdit(Transform);
        Position_Value_2->setObjectName(QString::fromUtf8("Position_Value_2"));
        Position_Value_2->setMaximumSize(QSize(40, 30));
        Position_Value_2->setDragEnabled(true);

        horizontalLayout_2->addWidget(Position_Value_2);

        pushButton_5 = new QPushButton(Transform);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setMaximumSize(QSize(30, 30));
        pushButton_5->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 255);"));

        horizontalLayout_2->addWidget(pushButton_5);

        lineEdit_3 = new QLineEdit(Transform);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(lineEdit_3);

        pushButton_6 = new QPushButton(Transform);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setMaximumSize(QSize(30, 30));
        pushButton_6->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 150, 225, 255);"));

        horizontalLayout_2->addWidget(pushButton_6);

        lineEdit_4 = new QLineEdit(Transform);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(lineEdit_4);


        verticalLayout_2->addLayout(horizontalLayout_2);

        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/transform_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolBox->addItem(Transform, icon, QString::fromUtf8("Transform"));

        verticalLayout->addWidget(toolBox);


        retranslateUi(TransformComponent);

        toolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TransformComponent);
    } // setupUi

    void retranslateUi(QWidget *TransformComponent)
    {
        TransformComponent->setWindowTitle(QCoreApplication::translate("TransformComponent", "Form", nullptr));
        Position_3->setText(QCoreApplication::translate("TransformComponent", "Position", nullptr));
        pushButton_7->setText(QCoreApplication::translate("TransformComponent", "X", nullptr));
        pushButton_8->setText(QCoreApplication::translate("TransformComponent", "Y", nullptr));
        pushButton_9->setText(QCoreApplication::translate("TransformComponent", "Z", nullptr));
        Position->setText(QCoreApplication::translate("TransformComponent", "Rotation", nullptr));
        pushButton->setText(QCoreApplication::translate("TransformComponent", "X", nullptr));
        pushButton_2->setText(QCoreApplication::translate("TransformComponent", "Y", nullptr));
        pushButton_3->setText(QCoreApplication::translate("TransformComponent", "Z", nullptr));
        Position_2->setText(QCoreApplication::translate("TransformComponent", "Scale", nullptr));
        pushButton_4->setText(QCoreApplication::translate("TransformComponent", "X", nullptr));
        pushButton_5->setText(QCoreApplication::translate("TransformComponent", "Y", nullptr));
        pushButton_6->setText(QCoreApplication::translate("TransformComponent", "Z", nullptr));
        toolBox->setItemText(toolBox->indexOf(Transform), QCoreApplication::translate("TransformComponent", "Transform", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TransformComponent: public Ui_TransformComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZETRANSFORMCOMPONENTUI_H
