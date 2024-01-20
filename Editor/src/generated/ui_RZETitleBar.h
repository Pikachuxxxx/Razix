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

QT_BEGIN_NAMESPACE

class Ui_TitleBar
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QFrame *EngineLogo;
    QVBoxLayout *menuPlaceHolder;
    QHBoxLayout *menuHolderLayout;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *ProjectName;
    QSpacerItem *horizontalSpacer_2;
    QLabel *BuildVersion;
    QPushButton *minimizeBtn;
    QPushButton *maximizeBtn;
    QPushButton *closeBtn;
    QSpacerItem *verticalSpacer;

    void setupUi(QFrame *TitleBar)
    {
        if (TitleBar->objectName().isEmpty())
            TitleBar->setObjectName(QString::fromUtf8("TitleBar"));
        TitleBar->resize(1255, 66);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TitleBar->sizePolicy().hasHeightForWidth());
        TitleBar->setSizePolicy(sizePolicy);
        TitleBar->setMinimumSize(QSize(0, 66));
        TitleBar->setMaximumSize(QSize(16777215, 66));
        horizontalLayout = new QHBoxLayout(TitleBar);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        EngineLogo = new QFrame(TitleBar);
        EngineLogo->setObjectName(QString::fromUtf8("EngineLogo"));
        EngineLogo->setMinimumSize(QSize(64, 64));
        EngineLogo->setMaximumSize(QSize(64, 64));
        EngineLogo->setAutoFillBackground(false);
        EngineLogo->setStyleSheet(QString::fromUtf8("image: url(:/rzeditor/RazixLogo64.png);"));
        EngineLogo->setFrameShape(QFrame::StyledPanel);
        EngineLogo->setFrameShadow(QFrame::Raised);

        verticalLayout_2->addWidget(EngineLogo);


        horizontalLayout->addLayout(verticalLayout_2);

        menuPlaceHolder = new QVBoxLayout();
        menuPlaceHolder->setSpacing(0);
        menuPlaceHolder->setObjectName(QString::fromUtf8("menuPlaceHolder"));
        menuHolderLayout = new QHBoxLayout();
        menuHolderLayout->setSpacing(0);
        menuHolderLayout->setObjectName(QString::fromUtf8("menuHolderLayout"));
        menuHolderLayout->setSizeConstraint(QLayout::SetMinimumSize);

        menuPlaceHolder->addLayout(menuHolderLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        menuPlaceHolder->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(menuPlaceHolder);

        horizontalSpacer = new QSpacerItem(494, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        ProjectName = new QLabel(TitleBar);
        ProjectName->setObjectName(QString::fromUtf8("ProjectName"));
        QFont font;
        font.setPointSize(12);
        ProjectName->setFont(font);
        ProjectName->setAcceptDrops(false);
        ProjectName->setToolTipDuration(2);
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

        BuildVersion = new QLabel(TitleBar);
        BuildVersion->setObjectName(QString::fromUtf8("BuildVersion"));
        BuildVersion->setFont(font);
        BuildVersion->setToolTipDuration(2);
        BuildVersion->setMargin(4);

        horizontalLayout_2->addWidget(BuildVersion);

        minimizeBtn = new QPushButton(TitleBar);
        minimizeBtn->setObjectName(QString::fromUtf8("minimizeBtn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        minimizeBtn->setIcon(icon);
        minimizeBtn->setIconSize(QSize(16, 16));
        minimizeBtn->setFlat(false);

        horizontalLayout_2->addWidget(minimizeBtn);

        maximizeBtn = new QPushButton(TitleBar);
        maximizeBtn->setObjectName(QString::fromUtf8("maximizeBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rzeditor/maximize.png"), QSize(), QIcon::Normal, QIcon::Off);
        maximizeBtn->setIcon(icon1);
        maximizeBtn->setIconSize(QSize(16, 16));
        maximizeBtn->setFlat(false);

        horizontalLayout_2->addWidget(maximizeBtn);

        closeBtn = new QPushButton(TitleBar);
        closeBtn->setObjectName(QString::fromUtf8("closeBtn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/rzeditor/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        closeBtn->setIcon(icon2);
        closeBtn->setIconSize(QSize(16, 16));
        closeBtn->setFlat(false);

        horizontalLayout_2->addWidget(closeBtn);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(TitleBar);

        QMetaObject::connectSlotsByName(TitleBar);
    } // setupUi

    void retranslateUi(QFrame *TitleBar)
    {
        TitleBar->setWindowTitle(QCoreApplication::translate("TitleBar", "RZETitleBar", nullptr));
#if QT_CONFIG(tooltip)
        ProjectName->setToolTip(QCoreApplication::translate("TitleBar", "Project Name", nullptr));
#endif // QT_CONFIG(tooltip)
        ProjectName->setText(QCoreApplication::translate("TitleBar", "ProjectName", nullptr));
#if QT_CONFIG(tooltip)
        BuildVersion->setToolTip(QCoreApplication::translate("TitleBar", "Engine Version", nullptr));
#endif // QT_CONFIG(tooltip)
        BuildVersion->setText(QCoreApplication::translate("TitleBar", "Build : V.0.43.0.Dev", nullptr));
        minimizeBtn->setText(QString());
        maximizeBtn->setText(QString());
        closeBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TitleBar: public Ui_TitleBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZETITLEBAR_H
