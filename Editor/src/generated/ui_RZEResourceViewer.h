/********************************************************************************
** Form generated from reading UI file 'RZEResourceViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZERESOURCEVIEWER_H
#define UI_RZERESOURCEVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResourceViewer
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *PoolGroup;
    QVBoxLayout *PoolGroupVLayout;
    QGroupBox *ResourceListGroup;
    QHBoxLayout *horizontalLayout;
    QTableWidget *ResourcesTable;

    void setupUi(QWidget *ResourceViewer)
    {
        if (ResourceViewer->objectName().isEmpty())
            ResourceViewer->setObjectName(QString::fromUtf8("ResourceViewer"));
        ResourceViewer->resize(1150, 541);
        verticalLayout = new QVBoxLayout(ResourceViewer);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        PoolGroup = new QGroupBox(ResourceViewer);
        PoolGroup->setObjectName(QString::fromUtf8("PoolGroup"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PoolGroup->sizePolicy().hasHeightForWidth());
        PoolGroup->setSizePolicy(sizePolicy);
        PoolGroup->setMaximumSize(QSize(16777215, 16777215));
        PoolGroupVLayout = new QVBoxLayout(PoolGroup);
        PoolGroupVLayout->setSpacing(6);
        PoolGroupVLayout->setContentsMargins(11, 11, 11, 11);
        PoolGroupVLayout->setObjectName(QString::fromUtf8("PoolGroupVLayout"));
        PoolGroupVLayout->setContentsMargins(9, 9, 9, 9);

        verticalLayout->addWidget(PoolGroup);

        ResourceListGroup = new QGroupBox(ResourceViewer);
        ResourceListGroup->setObjectName(QString::fromUtf8("ResourceListGroup"));
        horizontalLayout = new QHBoxLayout(ResourceListGroup);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(9, 9, 9, 9);
        ResourcesTable = new QTableWidget(ResourceListGroup);
        if (ResourcesTable->columnCount() < 10)
            ResourcesTable->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        ResourcesTable->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        ResourcesTable->setObjectName(QString::fromUtf8("ResourcesTable"));
        ResourcesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ResourcesTable->setWordWrap(false);
        ResourcesTable->setCornerButtonEnabled(false);
        ResourcesTable->setColumnCount(10);
        ResourcesTable->horizontalHeader()->setVisible(true);
        ResourcesTable->horizontalHeader()->setCascadingSectionResizes(false);
        ResourcesTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        ResourcesTable->horizontalHeader()->setStretchLastSection(true);
        ResourcesTable->verticalHeader()->setVisible(false);
        ResourcesTable->verticalHeader()->setCascadingSectionResizes(true);

        horizontalLayout->addWidget(ResourcesTable);


        verticalLayout->addWidget(ResourceListGroup);


        retranslateUi(ResourceViewer);

        QMetaObject::connectSlotsByName(ResourceViewer);
    } // setupUi

    void retranslateUi(QWidget *ResourceViewer)
    {
        ResourceViewer->setWindowTitle(QCoreApplication::translate("ResourceViewer", "Resource Viewer", nullptr));
        PoolGroup->setTitle(QCoreApplication::translate("ResourceViewer", "Resource Pools", nullptr));
        ResourceListGroup->setTitle(QCoreApplication::translate("ResourceViewer", "Resources List", nullptr));
        QTableWidgetItem *___qtablewidgetitem = ResourcesTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("ResourceViewer", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = ResourcesTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("ResourceViewer", "Pool", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = ResourcesTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("ResourceViewer", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = ResourcesTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("ResourceViewer", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = ResourcesTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("ResourceViewer", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = ResourcesTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("ResourceViewer", "CPU Memory", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = ResourcesTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("ResourceViewer", "GPU Memory", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = ResourcesTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("ResourceViewer", "Asset Location", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = ResourcesTable->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("ResourceViewer", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = ResourcesTable->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("ResourceViewer", "References", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ResourceViewer: public Ui_ResourceViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZERESOURCEVIEWER_H
