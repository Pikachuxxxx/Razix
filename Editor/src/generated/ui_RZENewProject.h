/********************************************************************************
** Form generated from reading UI file 'RZENewProject.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZENEWPROJECT_H
#define UI_RZENEWPROJECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RZENewProject
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *ProjectNameLineEdit;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *ProjectPath;
    QPushButton *BrowseBtn;
    QLabel *label_3;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *CreateBtn;
    QPushButton *CancelBtn;

    void setupUi(QDialog *RZENewProject)
    {
        if (RZENewProject->objectName().isEmpty())
            RZENewProject->setObjectName(QString::fromUtf8("RZENewProject"));
        RZENewProject->resize(400, 256);
        verticalLayout = new QVBoxLayout(RZENewProject);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(RZENewProject);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Fira Code"));
        label->setFont(font);

        verticalLayout->addWidget(label);

        ProjectNameLineEdit = new QLineEdit(RZENewProject);
        ProjectNameLineEdit->setObjectName(QString::fromUtf8("ProjectNameLineEdit"));

        verticalLayout->addWidget(ProjectNameLineEdit);

        label_2 = new QLabel(RZENewProject);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        verticalLayout->addWidget(label_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        ProjectPath = new QLineEdit(RZENewProject);
        ProjectPath->setObjectName(QString::fromUtf8("ProjectPath"));
        ProjectPath->setReadOnly(true);

        horizontalLayout_3->addWidget(ProjectPath);

        BrowseBtn = new QPushButton(RZENewProject);
        BrowseBtn->setObjectName(QString::fromUtf8("BrowseBtn"));

        horizontalLayout_3->addWidget(BrowseBtn);


        verticalLayout->addLayout(horizontalLayout_3);

        label_3 = new QLabel(RZENewProject);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);
        label_3->setWordWrap(true);

        verticalLayout->addWidget(label_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        CreateBtn = new QPushButton(RZENewProject);
        CreateBtn->setObjectName(QString::fromUtf8("CreateBtn"));

        horizontalLayout_2->addWidget(CreateBtn);

        CancelBtn = new QPushButton(RZENewProject);
        CancelBtn->setObjectName(QString::fromUtf8("CancelBtn"));

        horizontalLayout_2->addWidget(CancelBtn);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(RZENewProject);

        QMetaObject::connectSlotsByName(RZENewProject);
    } // setupUi

    void retranslateUi(QDialog *RZENewProject)
    {
        RZENewProject->setWindowTitle(QCoreApplication::translate("RZENewProject", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("RZENewProject", "Project Name:", nullptr));
        label_2->setText(QCoreApplication::translate("RZENewProject", "Project Path:", nullptr));
        BrowseBtn->setText(QCoreApplication::translate("RZENewProject", "Browse", nullptr));
        label_3->setText(QCoreApplication::translate("RZENewProject", "This is where the *.razixpjorect will be located as well as the Assets folder", nullptr));
        CreateBtn->setText(QCoreApplication::translate("RZENewProject", "Create", nullptr));
        CancelBtn->setText(QCoreApplication::translate("RZENewProject", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RZENewProject: public Ui_RZENewProject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZENEWPROJECT_H
