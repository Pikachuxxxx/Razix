/********************************************************************************
** Form generated from reading UI file 'RZETitleBar.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZETITLEBAR_H
#define UI_RZETITLEBAR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TitleBar
{
public:
    QVBoxLayout *verticalLayout_4;
    QFrame *header;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *ProjectName;
    QSpacerItem *horizontalSpacer_2;
    QLabel *BuildVersion;
    QPushButton *minimize;
    QPushButton *maximize;
    QPushButton *close;
    QSpacerItem *verticalSpacer;
    QWidget *body;
    QVBoxLayout *verticalLayout_6;

    void setupUi(QFrame *TitleBar)
    {
        if (TitleBar->objectName().isEmpty())
            TitleBar->setObjectName(QString::fromUtf8("TitleBar"));
        TitleBar->resize(1122, 66);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TitleBar->sizePolicy().hasHeightForWidth());
        TitleBar->setSizePolicy(sizePolicy);
        TitleBar->setMinimumSize(QSize(0, 66));
        TitleBar->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_4 = new QVBoxLayout(TitleBar);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        header = new QFrame(TitleBar);
        header->setObjectName(QString::fromUtf8("header"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(header->sizePolicy().hasHeightForWidth());
        header->setSizePolicy(sizePolicy1);
        header->setMinimumSize(QSize(0, 66));
        header->setMaximumSize(QSize(16777215, 66));
        header->setFrameShape(QFrame::StyledPanel);
        header->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(header);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        frame = new QFrame(header);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(64, 64));
        frame->setMaximumSize(QSize(64, 64));
        frame->setAutoFillBackground(false);
        frame->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/RazixLogo64.png);"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        verticalLayout_2->addWidget(frame);


        horizontalLayout_3->addLayout(verticalLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        ProjectName = new QLabel(header);
        ProjectName->setObjectName(QString::fromUtf8("ProjectName"));
        QFont font;
        font.setPointSize(12);
        ProjectName->setFont(font);
        ProjectName->setStyleSheet(QString::fromUtf8("#ProjectName{\n"
"background-color: rgb(16, 16, 16);\n"
"border-bottom-right-radius: 5px;\n"
"border-bottom-left-radius: 5px;\n"
"}\n"
""));
        ProjectName->setMargin(5);

        horizontalLayout_2->addWidget(ProjectName);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        BuildVersion = new QLabel(header);
        BuildVersion->setObjectName(QString::fromUtf8("BuildVersion"));
        BuildVersion->setFont(font);
        BuildVersion->setMargin(4);

        horizontalLayout_2->addWidget(BuildVersion);

        minimize = new QPushButton(header);
        minimize->setObjectName(QString::fromUtf8("minimize"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        minimize->setIcon(icon);

        horizontalLayout_2->addWidget(minimize);

        maximize = new QPushButton(header);
        maximize->setObjectName(QString::fromUtf8("maximize"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rzeditor/maximize.png"), QSize(), QIcon::Normal, QIcon::Off);
        maximize->setIcon(icon1);

        horizontalLayout_2->addWidget(maximize);

        close = new QPushButton(header);
        close->setObjectName(QString::fromUtf8("close"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/rzeditor/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        close->setIcon(icon2);

        horizontalLayout_2->addWidget(close);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_4->addWidget(header);

        body = new QWidget(TitleBar);
        body->setObjectName(QString::fromUtf8("body"));
        verticalLayout_6 = new QVBoxLayout(body);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);

        verticalLayout_4->addWidget(body);


        retranslateUi(TitleBar);

        QMetaObject::connectSlotsByName(TitleBar);
    } // setupUi

    void retranslateUi(QFrame *TitleBar)
    {
        TitleBar->setWindowTitle(QCoreApplication::translate("TitleBar", "RZETitleBar", nullptr));
        ProjectName->setText(QCoreApplication::translate("TitleBar", "ProjectName", nullptr));
        BuildVersion->setText(QCoreApplication::translate("TitleBar", "Build : V.0.43.0.Dev", nullptr));
        minimize->setText(QString());
        maximize->setText(QString());
        close->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TitleBar: public Ui_TitleBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZETITLEBAR_H
