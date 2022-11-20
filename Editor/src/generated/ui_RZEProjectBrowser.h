/********************************************************************************
** Form generated from reading UI file 'RZEProjectBrowser.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEPROJECTBROWSER_H
#define UI_RZEPROJECTBROWSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RZEProjectBrowser
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QPushButton *NewProjectBtn;
    QPushButton *OpenProjectBtn;
    QListWidget *projects_history_list;

    void setupUi(QDialog *RZEProjectBrowser)
    {
        if (RZEProjectBrowser->objectName().isEmpty())
            RZEProjectBrowser->setObjectName(QString::fromUtf8("RZEProjectBrowser"));
        RZEProjectBrowser->resize(753, 400);
        verticalLayout = new QVBoxLayout(RZEProjectBrowser);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(RZEProjectBrowser);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMaximumSize(QSize(16777215, 80));
        frame->setStyleSheet(QString::fromUtf8("background-color: rgb(83, 83, 83);"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Fira Code"));
        font.setPointSize(24);
        label->setFont(font);

        horizontalLayout->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        NewProjectBtn = new QPushButton(frame);
        NewProjectBtn->setObjectName(QString::fromUtf8("NewProjectBtn"));
        NewProjectBtn->setMinimumSize(QSize(60, 60));
        NewProjectBtn->setAutoFillBackground(false);
        NewProjectBtn->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/new_project.png);background-color: rgba(255, 255, 255, 0);"));
        NewProjectBtn->setFlat(true);

        horizontalLayout->addWidget(NewProjectBtn);

        OpenProjectBtn = new QPushButton(frame);
        OpenProjectBtn->setObjectName(QString::fromUtf8("OpenProjectBtn"));
        OpenProjectBtn->setMinimumSize(QSize(60, 60));
        OpenProjectBtn->setAutoFillBackground(false);
        OpenProjectBtn->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/open_project.png);\n"
"background-color: rgba(255, 255, 255, 0);"));
        OpenProjectBtn->setFlat(true);

        horizontalLayout->addWidget(OpenProjectBtn);


        verticalLayout->addWidget(frame);

        projects_history_list = new QListWidget(RZEProjectBrowser);
        projects_history_list->setObjectName(QString::fromUtf8("projects_history_list"));

        verticalLayout->addWidget(projects_history_list);


        retranslateUi(RZEProjectBrowser);

        QMetaObject::connectSlotsByName(RZEProjectBrowser);
    } // setupUi

    void retranslateUi(QDialog *RZEProjectBrowser)
    {
        RZEProjectBrowser->setWindowTitle(QCoreApplication::translate("RZEProjectBrowser", "RZEProjectBrowser", nullptr));
        label->setText(QCoreApplication::translate("RZEProjectBrowser", "Project Browser", nullptr));
        NewProjectBtn->setText(QString());
        OpenProjectBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class RZEProjectBrowser: public Ui_RZEProjectBrowser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEPROJECTBROWSER_H
