/********************************************************************************
** Form generated from reading UI file 'RZESceneHierarchyPanel.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZESCENEHIERARCHYPANEL_H
#define UI_RZESCENEHIERARCHYPANEL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SceneHierarchyPanel
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QSpacerItem *horizontalSpacer;
    QFrame *line;
    QTreeWidget *sceneTree;
    QSpacerItem *verticalSpacer;

    void setupUi(QFrame *SceneHierarchyPanel)
    {
        if (SceneHierarchyPanel->objectName().isEmpty())
            SceneHierarchyPanel->setObjectName(QString::fromUtf8("SceneHierarchyPanel"));
        SceneHierarchyPanel->resize(506, 767);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SceneHierarchyPanel->sizePolicy().hasHeightForWidth());
        SceneHierarchyPanel->setSizePolicy(sizePolicy);
        SceneHierarchyPanel->setMinimumSize(QSize(200, 100));
        SceneHierarchyPanel->setStyleSheet(QString::fromUtf8("QTreeView::branch:has-siblings:!adjoins-item {\n"
"    border-image: url(:/rzeditor/vline.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-more.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:!has-children:!has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-end.png) 0;\n"
"}\n"
"\n"
"QTreeView::branch:has-children:!has-siblings:closed,\n"
"QTreeView::branch:closed:has-children:has-siblings {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-closed.png);\n"
"}\n"
"\n"
"QTreeView::branch:open:has-children:!has-siblings,\n"
"QTreeView::branch:open:has-children:has-siblings  {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-open.png);\n"
"}"));
        verticalLayout = new QVBoxLayout(SceneHierarchyPanel);
        verticalLayout->setSpacing(10);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit = new QLineEdit(SceneHierarchyPanel);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        lineEdit->setFont(font);
        lineEdit->setStyleSheet(QString::fromUtf8(""));
        lineEdit->setClearButtonEnabled(true);

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(SceneHierarchyPanel);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        pushButton->setMinimumSize(QSize(16, 16));
        pushButton->setSizeIncrement(QSize(16, 16));
        QFont font1;
        font1.setPointSize(10);
        pushButton->setFont(font1);
        pushButton->setToolTipDuration(2);
        pushButton->setAutoFillBackground(false);
        pushButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/FilterDropdownIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(32, 32));
        pushButton->setFlat(true);

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(SceneHierarchyPanel);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(32, 32));
        label->setStyleSheet(QString::fromUtf8("background-image: url(:/rzeditor/WorldIcon.png) 0 0 0 0 stretch stretch;\n"
"background-repeat:no-repeat;\n"
"background-position: center;\n"
""));

        horizontalLayout_2->addWidget(label);

        lineEdit_2 = new QLineEdit(SceneHierarchyPanel);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setMinimumSize(QSize(200, 0));
        lineEdit_2->setFont(font1);
        lineEdit_2->setStyleSheet(QString::fromUtf8(""));
        lineEdit_2->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEdit_2);

        pushButton_2 = new QPushButton(SceneHierarchyPanel);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(50, 25));

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(SceneHierarchyPanel);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setMinimumSize(QSize(50, 25));

        horizontalLayout_2->addWidget(pushButton_3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        line = new QFrame(SceneHierarchyPanel);
        line->setObjectName(QString::fromUtf8("line"));
        line->setStyleSheet(QString::fromUtf8("background-color: rgb(172, 172, 172);"));
        line->setFrameShadow(QFrame::Plain);
        line->setLineWidth(2);
        line->setFrameShape(QFrame::HLine);

        verticalLayout->addWidget(line);

        sceneTree = new QTreeWidget(SceneHierarchyPanel);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rzeditor/RootIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setIcon(0, icon1);
        sceneTree->setHeaderItem(__qtreewidgetitem);
        sceneTree->setObjectName(QString::fromUtf8("sceneTree"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(sceneTree->sizePolicy().hasHeightForWidth());
        sceneTree->setSizePolicy(sizePolicy2);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Yu Gothic UI"));
        font2.setPointSize(10);
        sceneTree->setFont(font2);
        sceneTree->setStyleSheet(QString::fromUtf8("QTreeWidget::branch:has-siblings:!adjoins-item {\n"
"    border-image: url(:/rzeditor/vline.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-more.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:!has-children:!has-siblings:adjoins-item {\n"
"    border-image: url(:/rzeditor/branch-end.png) 0;\n"
"}\n"
"\n"
"QTreeWidget::branch:has-children:!has-siblings:closed,\n"
"QTreeWidget::branch:closed:has-children:has-siblings {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-closed.png);\n"
"}\n"
"\n"
"QTreeWidget::branch:open:has-children:!has-siblings,\n"
"QTreeWidget::branch:open:has-children:has-siblings  {\n"
"        border-image: none;\n"
"        image: url(:/rzeditor/branch-open.png);\n"
"}"));
        sceneTree->setDragDropMode(QAbstractItemView::InternalMove);
        sceneTree->setDefaultDropAction(Qt::IgnoreAction);
        sceneTree->setAlternatingRowColors(true);
        sceneTree->setIndentation(25);
        sceneTree->setAnimated(true);
        sceneTree->header()->setVisible(true);
        sceneTree->header()->setMinimumSectionSize(50);

        verticalLayout->addWidget(sceneTree);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(SceneHierarchyPanel);

        pushButton->setDefault(false);


        QMetaObject::connectSlotsByName(SceneHierarchyPanel);
    } // setupUi

    void retranslateUi(QFrame *SceneHierarchyPanel)
    {
        SceneHierarchyPanel->setWindowTitle(QCoreApplication::translate("SceneHierarchyPanel", "Scene Hierarchy Panel", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("SceneHierarchyPanel", "Search...", nullptr));
#if QT_CONFIG(tooltip)
        pushButton->setToolTip(QCoreApplication::translate("SceneHierarchyPanel", "Filter the items", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton->setText(QString());
        label->setText(QString());
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("SceneHierarchyPanel", "Browse to choose *.world file", nullptr));
        pushButton_2->setText(QCoreApplication::translate("SceneHierarchyPanel", "New", nullptr));
        pushButton_3->setText(QCoreApplication::translate("SceneHierarchyPanel", "Edit", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = sceneTree->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("SceneHierarchyPanel", "Root", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneHierarchyPanel: public Ui_SceneHierarchyPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZESCENEHIERARCHYPANEL_H
