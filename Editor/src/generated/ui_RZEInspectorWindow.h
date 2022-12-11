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
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InspectorWindow
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *TagIcon;
    QLineEdit *EntityName;
    QCheckBox *IsStatic;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *UUIDTitle;
    QLabel *UUIDLbl;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *InspectorWindow)
    {
        if (InspectorWindow->objectName().isEmpty())
            InspectorWindow->setObjectName(QString::fromUtf8("InspectorWindow"));
        InspectorWindow->resize(469, 311);
        InspectorWindow->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(InspectorWindow);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        scrollArea = new QScrollArea(InspectorWindow);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Sunken);
        scrollArea->setLineWidth(1);
        scrollArea->setWidgetResizable(true);
        scrollWidget = new QWidget();
        scrollWidget->setObjectName(QString::fromUtf8("scrollWidget"));
        scrollWidget->setGeometry(QRect(0, 0, 459, 301));
        scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setSpacing(5);
        scrollLayout->setObjectName(QString::fromUtf8("scrollLayout"));
        scrollLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 5, -1, 5);
        TagIcon = new QLabel(scrollWidget);
        TagIcon->setObjectName(QString::fromUtf8("TagIcon"));
        TagIcon->setMaximumSize(QSize(25, 25));
        TagIcon->setPixmap(QPixmap(QString::fromUtf8(":/rzeditor/tag_icon.png")));
        TagIcon->setScaledContents(true);

        horizontalLayout->addWidget(TagIcon);

        EntityName = new QLineEdit(scrollWidget);
        EntityName->setObjectName(QString::fromUtf8("EntityName"));

        horizontalLayout->addWidget(EntityName);

        IsStatic = new QCheckBox(scrollWidget);
        IsStatic->setObjectName(QString::fromUtf8("IsStatic"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IsStatic->sizePolicy().hasHeightForWidth());
        IsStatic->setSizePolicy(sizePolicy);
        IsStatic->setMaximumSize(QSize(50, 25));

        horizontalLayout->addWidget(IsStatic);

        checkBox = new QCheckBox(scrollWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        horizontalLayout->addWidget(checkBox);


        scrollLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        UUIDTitle = new QLabel(scrollWidget);
        UUIDTitle->setObjectName(QString::fromUtf8("UUIDTitle"));
        UUIDTitle->setMaximumSize(QSize(60, 16777215));
        QFont font;
        font.setPointSize(12);
        UUIDTitle->setFont(font);
        UUIDTitle->setStyleSheet(QString::fromUtf8("color: rgb(161, 161, 161);border:0px;"));

        horizontalLayout_2->addWidget(UUIDTitle);

        UUIDLbl = new QLabel(scrollWidget);
        UUIDLbl->setObjectName(QString::fromUtf8("UUIDLbl"));
        UUIDLbl->setMinimumSize(QSize(0, 25));
        UUIDLbl->setMaximumSize(QSize(16777215, 25));
        UUIDLbl->setStyleSheet(QString::fromUtf8("border-color: rgb(147, 147, 147);\n"
"border:1px;\n"
"border-radius:5;\n"
"border-style:solid;"));

        horizontalLayout_2->addWidget(UUIDLbl);


        scrollLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        scrollLayout->addItem(verticalSpacer);

        scrollArea->setWidget(scrollWidget);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(InspectorWindow);

        QMetaObject::connectSlotsByName(InspectorWindow);
    } // setupUi

    void retranslateUi(QWidget *InspectorWindow)
    {
        InspectorWindow->setWindowTitle(QCoreApplication::translate("InspectorWindow", "Form", nullptr));
        TagIcon->setText(QString());
        EntityName->setText(QCoreApplication::translate("InspectorWindow", "Character", nullptr));
        IsStatic->setText(QCoreApplication::translate("InspectorWindow", "Static", nullptr));
        checkBox->setText(QCoreApplication::translate("InspectorWindow", "Active", nullptr));
        UUIDTitle->setText(QCoreApplication::translate("InspectorWindow", "UUID :", nullptr));
        UUIDLbl->setText(QCoreApplication::translate("InspectorWindow", "123e4567-e89b-12d3-a456-426614174000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InspectorWindow: public Ui_InspectorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEINSPECTORWINDOW_H
