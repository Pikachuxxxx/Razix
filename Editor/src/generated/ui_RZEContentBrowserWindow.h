/********************************************************************************
** Form generated from reading UI file 'RZEContentBrowserWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZECONTENTBROWSERWINDOW_H
#define UI_RZECONTENTBROWSERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_ContentBrowserWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QTreeView *directoryTreeView;
    QListView *folderListView;

    void setupUi(QFrame *ContentBrowserWindow)
    {
        if (ContentBrowserWindow->objectName().isEmpty())
            ContentBrowserWindow->setObjectName(QString::fromUtf8("ContentBrowserWindow"));
        ContentBrowserWindow->resize(600, 400);
        horizontalLayout = new QHBoxLayout(ContentBrowserWindow);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter = new QSplitter(ContentBrowserWindow);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        directoryTreeView = new QTreeView(splitter);
        directoryTreeView->setObjectName(QString::fromUtf8("directoryTreeView"));
        splitter->addWidget(directoryTreeView);
        folderListView = new QListView(splitter);
        folderListView->setObjectName(QString::fromUtf8("folderListView"));
        splitter->addWidget(folderListView);

        horizontalLayout->addWidget(splitter);


        retranslateUi(ContentBrowserWindow);

        QMetaObject::connectSlotsByName(ContentBrowserWindow);
    } // setupUi

    void retranslateUi(QFrame *ContentBrowserWindow)
    {
        ContentBrowserWindow->setWindowTitle(QCoreApplication::translate("ContentBrowserWindow", "Content Browser", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ContentBrowserWindow: public Ui_ContentBrowserWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZECONTENTBROWSERWINDOW_H
