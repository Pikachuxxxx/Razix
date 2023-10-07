/********************************************************************************
** Form generated from reading UI file 'RZEFrameGraphEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RZEFRAMEGRAPHEDITOR_H
#define UI_RZEFRAMEGRAPHEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FrameGraphEditor
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *toolbar;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *open_fg;
    QLabel *label;
    QVBoxLayout *verticalLayout_3;
    QPushButton *save_fg;
    QLabel *label_2;
    QVBoxLayout *verticalLayout_5;
    QPushButton *undo;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_6;
    QPushButton *redo;
    QLabel *label_5;
    QFrame *line;
    QVBoxLayout *verticalLayout_4;
    QPushButton *compile;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_7;
    QPushButton *hot_reload;
    QLabel *label_6;
    QFrame *line_2;
    QVBoxLayout *verticalLayout_8;
    QPushButton *group;
    QLabel *label_7;
    QVBoxLayout *verticalLayout_9;
    QPushButton *import_2;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_10;
    QPushButton *debug;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_11;
    QPushButton *add_code_pass;
    QLabel *label_10;
    QVBoxLayout *verticalLayout_12;
    QPushButton *search;
    QLabel *label_11;
    QVBoxLayout *verticalLayout_13;
    QPushButton *blackboard;
    QLabel *label_12;
    QVBoxLayout *verticalLayout_14;
    QPushButton *add_to_blackboard;
    QLabel *label_13;
    QVBoxLayout *verticalLayout_15;
    QPushButton *disable_pass;
    QLabel *label_14;
    QVBoxLayout *verticalLayout_17;
    QPushButton *mask_as_standalone;
    QLabel *label_16;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_18;
    QPushButton *mask_as_standalone_2;
    QLabel *label_17;
    QHBoxLayout *horizontalLayout;
    QFrame *presets_inspector;
    QVBoxLayout *verticalLayout_16;
    QLabel *PassPresetsLbl;
    QWidget *test_preset;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_15;
    QLabel *label_18;
    QWidget *widget;
    QVBoxLayout *verticalLayout_19;
    QListWidget *preset_passes_list;
    QLabel *label_19;
    QListWidget *preset_import_list;
    QLabel *label_20;
    QListWidget *preset_resources_list;
    QVBoxLayout *body_layout;
    QFrame *node_inspector;

    void setupUi(QWidget *FrameGraphEditor)
    {
        if (FrameGraphEditor->objectName().isEmpty())
            FrameGraphEditor->setObjectName(QString::fromUtf8("FrameGraphEditor"));
        FrameGraphEditor->resize(1366, 923);
        FrameGraphEditor->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(FrameGraphEditor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolbar = new QFrame(FrameGraphEditor);
        toolbar->setObjectName(QString::fromUtf8("toolbar"));
        toolbar->setMinimumSize(QSize(0, 75));
        toolbar->setMaximumSize(QSize(16777215, 75));
        toolbar->setFrameShape(QFrame::StyledPanel);
        toolbar->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(toolbar);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        open_fg = new QPushButton(toolbar);
        open_fg->setObjectName(QString::fromUtf8("open_fg"));
        open_fg->setMinimumSize(QSize(32, 40));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        open_fg->setIcon(icon);
        open_fg->setIconSize(QSize(32, 32));
        open_fg->setFlat(true);

        verticalLayout_2->addWidget(open_fg);

        label = new QLabel(toolbar);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        save_fg = new QPushButton(toolbar);
        save_fg->setObjectName(QString::fromUtf8("save_fg"));
        save_fg->setMinimumSize(QSize(32, 40));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        save_fg->setIcon(icon1);
        save_fg->setIconSize(QSize(32, 32));
        save_fg->setFlat(true);

        verticalLayout_3->addWidget(save_fg);

        label_2 = new QLabel(toolbar);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_2);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        undo = new QPushButton(toolbar);
        undo->setObjectName(QString::fromUtf8("undo"));
        undo->setMinimumSize(QSize(32, 40));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        undo->setIcon(icon2);
        undo->setIconSize(QSize(32, 32));
        undo->setFlat(true);

        verticalLayout_5->addWidget(undo);

        label_4 = new QLabel(toolbar);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_4);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        redo = new QPushButton(toolbar);
        redo->setObjectName(QString::fromUtf8("redo"));
        redo->setMinimumSize(QSize(32, 40));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        redo->setIcon(icon3);
        redo->setIconSize(QSize(32, 32));
        redo->setFlat(true);

        verticalLayout_6->addWidget(redo);

        label_5 = new QLabel(toolbar);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(label_5);


        horizontalLayout_2->addLayout(verticalLayout_6);

        line = new QFrame(toolbar);
        line->setObjectName(QString::fromUtf8("line"));
        line->setStyleSheet(QString::fromUtf8("line:{\n"
"color:#ff00ff\n"
"}"));
        line->setFrameShadow(QFrame::Plain);
        line->setLineWidth(3);
        line->setMidLineWidth(0);
        line->setFrameShape(QFrame::VLine);

        horizontalLayout_2->addWidget(line);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        compile = new QPushButton(toolbar);
        compile->setObjectName(QString::fromUtf8("compile"));
        compile->setMinimumSize(QSize(32, 40));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/compile.png"), QSize(), QIcon::Normal, QIcon::Off);
        compile->setIcon(icon4);
        compile->setIconSize(QSize(32, 32));
        compile->setFlat(true);

        verticalLayout_4->addWidget(compile);

        label_3 = new QLabel(toolbar);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(label_3);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        hot_reload = new QPushButton(toolbar);
        hot_reload->setObjectName(QString::fromUtf8("hot_reload"));
        hot_reload->setMinimumSize(QSize(32, 40));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/hot_reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        hot_reload->setIcon(icon5);
        hot_reload->setIconSize(QSize(32, 32));
        hot_reload->setFlat(true);

        verticalLayout_7->addWidget(hot_reload);

        label_6 = new QLabel(toolbar);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(label_6);


        horizontalLayout_2->addLayout(verticalLayout_7);

        line_2 = new QFrame(toolbar);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setAutoFillBackground(false);
        line_2->setStyleSheet(QString::fromUtf8("line_2:{\n"
"    background-color: rgb(255, 255, 255);\n"
"    width: 5px;\n"
"    border-width: 5px;\n"
"    border-color: #ffffff;\n"
"}"));
        line_2->setFrameShadow(QFrame::Plain);
        line_2->setLineWidth(3);
        line_2->setFrameShape(QFrame::VLine);

        horizontalLayout_2->addWidget(line_2);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        group = new QPushButton(toolbar);
        group->setObjectName(QString::fromUtf8("group"));
        group->setMinimumSize(QSize(32, 40));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/group.png"), QSize(), QIcon::Normal, QIcon::Off);
        group->setIcon(icon6);
        group->setIconSize(QSize(32, 32));
        group->setFlat(true);

        verticalLayout_8->addWidget(group);

        label_7 = new QLabel(toolbar);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(label_7);


        horizontalLayout_2->addLayout(verticalLayout_8);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        import_2 = new QPushButton(toolbar);
        import_2->setObjectName(QString::fromUtf8("import_2"));
        import_2->setMinimumSize(QSize(32, 40));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/import.png"), QSize(), QIcon::Normal, QIcon::Off);
        import_2->setIcon(icon7);
        import_2->setIconSize(QSize(32, 32));
        import_2->setFlat(true);

        verticalLayout_9->addWidget(import_2);

        label_8 = new QLabel(toolbar);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(label_8);


        horizontalLayout_2->addLayout(verticalLayout_9);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        debug = new QPushButton(toolbar);
        debug->setObjectName(QString::fromUtf8("debug"));
        debug->setMinimumSize(QSize(32, 40));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/debug.png"), QSize(), QIcon::Normal, QIcon::Off);
        debug->setIcon(icon8);
        debug->setIconSize(QSize(32, 32));
        debug->setFlat(true);

        verticalLayout_10->addWidget(debug);

        label_9 = new QLabel(toolbar);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignCenter);

        verticalLayout_10->addWidget(label_9);


        horizontalLayout_2->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        add_code_pass = new QPushButton(toolbar);
        add_code_pass->setObjectName(QString::fromUtf8("add_code_pass"));
        add_code_pass->setMinimumSize(QSize(32, 40));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/add_code_pass.png"), QSize(), QIcon::Normal, QIcon::Off);
        add_code_pass->setIcon(icon9);
        add_code_pass->setIconSize(QSize(32, 32));
        add_code_pass->setFlat(true);

        verticalLayout_11->addWidget(add_code_pass);

        label_10 = new QLabel(toolbar);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAlignment(Qt::AlignCenter);
        label_10->setWordWrap(false);

        verticalLayout_11->addWidget(label_10);


        horizontalLayout_2->addLayout(verticalLayout_11);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        search = new QPushButton(toolbar);
        search->setObjectName(QString::fromUtf8("search"));
        search->setMinimumSize(QSize(32, 40));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/search.png"), QSize(), QIcon::Normal, QIcon::Off);
        search->setIcon(icon10);
        search->setIconSize(QSize(32, 32));
        search->setFlat(true);

        verticalLayout_12->addWidget(search);

        label_11 = new QLabel(toolbar);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(label_11);


        horizontalLayout_2->addLayout(verticalLayout_12);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        blackboard = new QPushButton(toolbar);
        blackboard->setObjectName(QString::fromUtf8("blackboard"));
        blackboard->setMinimumSize(QSize(32, 40));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/blackboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        blackboard->setIcon(icon11);
        blackboard->setIconSize(QSize(32, 32));
        blackboard->setFlat(true);

        verticalLayout_13->addWidget(blackboard);

        label_12 = new QLabel(toolbar);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setAlignment(Qt::AlignCenter);
        label_12->setWordWrap(false);

        verticalLayout_13->addWidget(label_12);


        horizontalLayout_2->addLayout(verticalLayout_13);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(0);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        add_to_blackboard = new QPushButton(toolbar);
        add_to_blackboard->setObjectName(QString::fromUtf8("add_to_blackboard"));
        add_to_blackboard->setMinimumSize(QSize(32, 40));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/add_blackboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        add_to_blackboard->setIcon(icon12);
        add_to_blackboard->setIconSize(QSize(32, 32));
        add_to_blackboard->setFlat(true);

        verticalLayout_14->addWidget(add_to_blackboard);

        label_13 = new QLabel(toolbar);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setAlignment(Qt::AlignCenter);
        label_13->setWordWrap(false);

        verticalLayout_14->addWidget(label_13);


        horizontalLayout_2->addLayout(verticalLayout_14);

        verticalLayout_15 = new QVBoxLayout();
        verticalLayout_15->setSpacing(0);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        disable_pass = new QPushButton(toolbar);
        disable_pass->setObjectName(QString::fromUtf8("disable_pass"));
        disable_pass->setMinimumSize(QSize(32, 40));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/disable_pass.png"), QSize(), QIcon::Normal, QIcon::Off);
        disable_pass->setIcon(icon13);
        disable_pass->setIconSize(QSize(32, 32));
        disable_pass->setFlat(true);

        verticalLayout_15->addWidget(disable_pass);

        label_14 = new QLabel(toolbar);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setAlignment(Qt::AlignCenter);
        label_14->setWordWrap(false);

        verticalLayout_15->addWidget(label_14);


        horizontalLayout_2->addLayout(verticalLayout_15);

        verticalLayout_17 = new QVBoxLayout();
        verticalLayout_17->setSpacing(0);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        mask_as_standalone = new QPushButton(toolbar);
        mask_as_standalone->setObjectName(QString::fromUtf8("mask_as_standalone"));
        mask_as_standalone->setMinimumSize(QSize(32, 40));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/mark_standalone_pass.png"), QSize(), QIcon::Normal, QIcon::Off);
        mask_as_standalone->setIcon(icon14);
        mask_as_standalone->setIconSize(QSize(32, 32));
        mask_as_standalone->setFlat(true);

        verticalLayout_17->addWidget(mask_as_standalone);

        label_16 = new QLabel(toolbar);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setScaledContents(true);
        label_16->setAlignment(Qt::AlignCenter);
        label_16->setWordWrap(true);

        verticalLayout_17->addWidget(label_16);


        horizontalLayout_2->addLayout(verticalLayout_17);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        verticalLayout_18 = new QVBoxLayout();
        verticalLayout_18->setSpacing(0);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        mask_as_standalone_2 = new QPushButton(toolbar);
        mask_as_standalone_2->setObjectName(QString::fromUtf8("mask_as_standalone_2"));
        mask_as_standalone_2->setMinimumSize(QSize(32, 40));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/rzeditor/fg_toolbar_icons/breakpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
        mask_as_standalone_2->setIcon(icon15);
        mask_as_standalone_2->setIconSize(QSize(32, 32));
        mask_as_standalone_2->setFlat(true);

        verticalLayout_18->addWidget(mask_as_standalone_2);

        label_17 = new QLabel(toolbar);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setAlignment(Qt::AlignCenter);
        label_17->setWordWrap(false);

        verticalLayout_18->addWidget(label_17);


        horizontalLayout_2->addLayout(verticalLayout_18);


        verticalLayout->addWidget(toolbar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        presets_inspector = new QFrame(FrameGraphEditor);
        presets_inspector->setObjectName(QString::fromUtf8("presets_inspector"));
        presets_inspector->setMinimumSize(QSize(250, 0));
        presets_inspector->setMaximumSize(QSize(300, 16777215));
        presets_inspector->setFrameShape(QFrame::StyledPanel);
        presets_inspector->setFrameShadow(QFrame::Raised);
        verticalLayout_16 = new QVBoxLayout(presets_inspector);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        verticalLayout_16->setContentsMargins(0, 0, 0, 0);
        PassPresetsLbl = new QLabel(presets_inspector);
        PassPresetsLbl->setObjectName(QString::fromUtf8("PassPresetsLbl"));
        PassPresetsLbl->setMinimumSize(QSize(0, 25));
        PassPresetsLbl->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setPointSize(16);
        font.setBold(false);
        font.setWeight(50);
        PassPresetsLbl->setFont(font);
        PassPresetsLbl->setAutoFillBackground(false);
        PassPresetsLbl->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;\n"
"\n"
""));
        PassPresetsLbl->setScaledContents(false);
        PassPresetsLbl->setAlignment(Qt::AlignCenter);
        PassPresetsLbl->setMargin(4);

        verticalLayout_16->addWidget(PassPresetsLbl);

        test_preset = new QWidget(presets_inspector);
        test_preset->setObjectName(QString::fromUtf8("test_preset"));
        test_preset->setMaximumSize(QSize(16777215, 50));
        test_preset->setAutoFillBackground(false);
        test_preset->setStyleSheet(QString::fromUtf8("background-color: #212121;\n"
"color: #FFFFFF;\n"
"border-color: #696969;\n"
"border-width:4px;\n"
"border-style: solid;\n"
"border-radius:20px;"));
        horizontalLayout_4 = new QHBoxLayout(test_preset);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_15 = new QLabel(test_preset);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Fira Code"));
        font1.setPointSize(16);
        label_15->setFont(font1);
        label_15->setStyleSheet(QString::fromUtf8("\n"
"color: #FFFFFF;\n"
"border-width:0px;\n"
"border-style: solid;\n"
"border-radius:0px;"));

        horizontalLayout_4->addWidget(label_15);

        label_18 = new QLabel(test_preset);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setMaximumSize(QSize(80, 16777215));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Fira Code"));
        font2.setPointSize(12);
        label_18->setFont(font2);
        label_18->setStyleSheet(QString::fromUtf8("background-color: #AAAAAA;\n"
"color: #000000;\n"
"border-color:#000000;\n"
"border-width:1px;\n"
"border-style: solid;\n"
"border-radius:14px;"));
        label_18->setAlignment(Qt::AlignCenter);
        label_18->setMargin(3);

        horizontalLayout_4->addWidget(label_18);


        verticalLayout_16->addWidget(test_preset);

        widget = new QWidget(presets_inspector);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setStyleSheet(QString::fromUtf8("background-color: #F6D975;\n"
"border-color: rgb(0, 0, 0);\n"
"border-width:1px;\n"
"border-style:dashed;\n"
"border-radius:4px;"));
        verticalLayout_19 = new QVBoxLayout(widget);
        verticalLayout_19->setSpacing(0);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        verticalLayout_19->setContentsMargins(0, 0, 0, 0);

        verticalLayout_16->addWidget(widget);

        preset_passes_list = new QListWidget(presets_inspector);
        preset_passes_list->setObjectName(QString::fromUtf8("preset_passes_list"));
        preset_passes_list->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 0, 0, 0);"));
        preset_passes_list->setDragEnabled(true);
        preset_passes_list->setSpacing(12);

        verticalLayout_16->addWidget(preset_passes_list);

        label_19 = new QLabel(presets_inspector);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        QFont font3;
        font3.setPointSize(16);
        label_19->setFont(font3);
        label_19->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;"));
        label_19->setAlignment(Qt::AlignCenter);

        verticalLayout_16->addWidget(label_19);

        preset_import_list = new QListWidget(presets_inspector);
        preset_import_list->setObjectName(QString::fromUtf8("preset_import_list"));
        preset_import_list->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 0, 0, 0);"));
        preset_import_list->setDragEnabled(true);
        preset_import_list->setSpacing(12);

        verticalLayout_16->addWidget(preset_import_list);

        label_20 = new QLabel(presets_inspector);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font3);
        label_20->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;"));
        label_20->setAlignment(Qt::AlignCenter);

        verticalLayout_16->addWidget(label_20);

        preset_resources_list = new QListWidget(presets_inspector);
        preset_resources_list->setObjectName(QString::fromUtf8("preset_resources_list"));
        preset_resources_list->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 0, 0, 0);"));
        preset_resources_list->setDragEnabled(true);
        preset_resources_list->setSpacing(12);

        verticalLayout_16->addWidget(preset_resources_list);


        horizontalLayout->addWidget(presets_inspector);

        body_layout = new QVBoxLayout();
        body_layout->setObjectName(QString::fromUtf8("body_layout"));

        horizontalLayout->addLayout(body_layout);

        node_inspector = new QFrame(FrameGraphEditor);
        node_inspector->setObjectName(QString::fromUtf8("node_inspector"));
        node_inspector->setFrameShape(QFrame::StyledPanel);
        node_inspector->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(node_inspector);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(FrameGraphEditor);

        QMetaObject::connectSlotsByName(FrameGraphEditor);
    } // setupUi

    void retranslateUi(QWidget *FrameGraphEditor)
    {
        FrameGraphEditor->setWindowTitle(QCoreApplication::translate("FrameGraphEditor", "Frame Graph Editor", nullptr));
        open_fg->setText(QString());
        label->setText(QCoreApplication::translate("FrameGraphEditor", "Open", nullptr));
        save_fg->setText(QString());
        label_2->setText(QCoreApplication::translate("FrameGraphEditor", "Save", nullptr));
        undo->setText(QString());
        label_4->setText(QCoreApplication::translate("FrameGraphEditor", "Undo", nullptr));
        redo->setText(QString());
        label_5->setText(QCoreApplication::translate("FrameGraphEditor", "Redo", nullptr));
        compile->setText(QString());
        label_3->setText(QCoreApplication::translate("FrameGraphEditor", "Compile", nullptr));
        hot_reload->setText(QString());
        label_6->setText(QCoreApplication::translate("FrameGraphEditor", "Hot Reload", nullptr));
        group->setText(QString());
        label_7->setText(QCoreApplication::translate("FrameGraphEditor", "Group", nullptr));
        import_2->setText(QString());
        label_8->setText(QCoreApplication::translate("FrameGraphEditor", "Import", nullptr));
        debug->setText(QString());
        label_9->setText(QCoreApplication::translate("FrameGraphEditor", "Debug", nullptr));
        add_code_pass->setText(QString());
        label_10->setText(QCoreApplication::translate("FrameGraphEditor", "Add code pass", nullptr));
        search->setText(QString());
        label_11->setText(QCoreApplication::translate("FrameGraphEditor", "Search", nullptr));
        blackboard->setText(QString());
        label_12->setText(QCoreApplication::translate("FrameGraphEditor", "Blackboard", nullptr));
        add_to_blackboard->setText(QString());
        label_13->setText(QCoreApplication::translate("FrameGraphEditor", "Add to Blackboad", nullptr));
        disable_pass->setText(QString());
        label_14->setText(QCoreApplication::translate("FrameGraphEditor", "Enable/Disable Pass", nullptr));
        mask_as_standalone->setText(QString());
        label_16->setText(QCoreApplication::translate("FrameGraphEditor", "Mark as Standalone", nullptr));
        mask_as_standalone_2->setText(QString());
        label_17->setText(QCoreApplication::translate("FrameGraphEditor", "Add Breakpoint", nullptr));
        PassPresetsLbl->setText(QCoreApplication::translate("FrameGraphEditor", "Pass Presets", nullptr));
        label_15->setText(QCoreApplication::translate("FrameGraphEditor", "Shadow Pass", nullptr));
        label_18->setText(QCoreApplication::translate("FrameGraphEditor", "{code}", nullptr));
        label_19->setText(QCoreApplication::translate("FrameGraphEditor", "Import Presets", nullptr));
        label_20->setText(QCoreApplication::translate("FrameGraphEditor", "Resource Node Presets", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FrameGraphEditor: public Ui_FrameGraphEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEFRAMEGRAPHEDITOR_H
