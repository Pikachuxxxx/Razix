/********************************************************************************
** Form generated from reading UI file 'RZEInspectorWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEINSPECTORWINDOW_H
#define UI_RZEINSPECTORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InspectorWindow
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *TagIcon;
    QLineEdit *EntityName;
    QCheckBox *IsStatic;
    QHBoxLayout *horizontalLayout_2;
    QLabel *UUIDLbl;
    QLabel *UUIDString;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *InspectorWindow)
    {
        if (InspectorWindow->objectName().isEmpty())
            InspectorWindow->setObjectName(QString::fromUtf8("InspectorWindow"));
        InspectorWindow->resize(412, 486);
        InspectorWindow->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(InspectorWindow);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 5, -1, 5);
        TagIcon = new QLabel(InspectorWindow);
        TagIcon->setObjectName(QString::fromUtf8("TagIcon"));
        TagIcon->setMaximumSize(QSize(25, 25));
        TagIcon->setPixmap(QPixmap(QString::fromUtf8(":/rzeditor/tag_icon.png")));
        TagIcon->setScaledContents(true);

        horizontalLayout->addWidget(TagIcon);

        EntityName = new QLineEdit(InspectorWindow);
        EntityName->setObjectName(QString::fromUtf8("EntityName"));

        horizontalLayout->addWidget(EntityName);

        IsStatic = new QCheckBox(InspectorWindow);
        IsStatic->setObjectName(QString::fromUtf8("IsStatic"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IsStatic->sizePolicy().hasHeightForWidth());
        IsStatic->setSizePolicy(sizePolicy);
        IsStatic->setMaximumSize(QSize(50, 25));

        horizontalLayout->addWidget(IsStatic);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        UUIDLbl = new QLabel(InspectorWindow);
        UUIDLbl->setObjectName(QString::fromUtf8("UUIDLbl"));
        UUIDLbl->setMaximumSize(QSize(60, 16777215));
        QFont font;
        font.setPointSize(12);
        UUIDLbl->setFont(font);
        UUIDLbl->setStyleSheet(QString::fromUtf8("color: rgb(161, 161, 161);border:0px;"));

        horizontalLayout_2->addWidget(UUIDLbl);

        UUIDString = new QLabel(InspectorWindow);
        UUIDString->setObjectName(QString::fromUtf8("UUIDString"));
        UUIDString->setStyleSheet(QString::fromUtf8("border-color: rgb(147, 147, 147);\n"
"border:1px;\n"
"border-radius:5;\n"
"border-style:solid;"));

        horizontalLayout_2->addWidget(UUIDString);


        verticalLayout_2->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        retranslateUi(InspectorWindow);

        QMetaObject::connectSlotsByName(InspectorWindow);
    } // setupUi

    void retranslateUi(QWidget *InspectorWindow)
    {
        InspectorWindow->setWindowTitle(QCoreApplication::translate("InspectorWindow", "Form", nullptr));
        TagIcon->setText(QString());
        EntityName->setText(QCoreApplication::translate("InspectorWindow", "Character", nullptr));
        IsStatic->setText(QCoreApplication::translate("InspectorWindow", "Static", nullptr));
        UUIDLbl->setText(QCoreApplication::translate("InspectorWindow", "UUID :", nullptr));
        UUIDString->setText(QCoreApplication::translate("InspectorWindow", "123e4567-e89b-12d3-a456-426614174000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InspectorWindow: public Ui_InspectorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEINSPECTORWINDOW_H
