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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
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
    QVBoxLayout *verticalLayout_18;
    QPushButton *mask_as_standalone_2;
    QLabel *label_17;
    QSpacerItem *horizontalSpacer;
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
    QStackedWidget *stackedWidget;
    QWidget *page_1_empty;
    QVBoxLayout *verticalLayout_20;
    QLabel *PassPresetsLbl_3;
    QLabel *label_21;
    QSpacerItem *verticalSpacer;
    QWidget *page_2_pass_node;
    QVBoxLayout *verticalLayout_23;
    QLabel *PassPresetsLbl_2;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_49;
    QLineEdit *PassName;
    QFrame *line_6;
    QLabel *label_48;
    QVBoxLayout *verticalLayout_22;
    QFormLayout *ip_pins_layout;
    QPushButton *add_ip_pin;
    QPushButton *remove_ip_pin;
    QFormLayout *op_pins_layout;
    QPushButton *add_op_pin;
    QPushButton *remove_op_pin;
    QFrame *line_5;
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *shaderFileLocation;
    QPushButton *browseShaderFIleBtn;
    QLabel *label_22;
    QFrame *line_3;
    QLabel *label_41;
    QFormLayout *formLayout_2;
    QLabel *label_24;
    QLineEdit *pipelineName;
    QLabel *label_25;
    QComboBox *cullMode;
    QLabel *label_26;
    QComboBox *polygonMode;
    QLabel *label_27;
    QComboBox *drawType;
    QLabel *label_28;
    QCheckBox *enableTransparency;
    QLabel *label_29;
    QCheckBox *enableDepthTest;
    QLabel *label_30;
    QCheckBox *enableDepthWrite;
    QLabel *label_38;
    QComboBox *depthOperation;
    QLabel *label_31;
    QComboBox *colorSrc;
    QLabel *label_32;
    QComboBox *colorDst;
    QLabel *label_36;
    QComboBox *colorOp;
    QLabel *label_33;
    QComboBox *alphaSrc;
    QLabel *label_34;
    QComboBox *alphaDst;
    QLabel *label_37;
    QComboBox *alphaOp;
    QLabel *label_35;
    QComboBox *depthFormat;
    QLabel *label_39;
    QVBoxLayout *verticalLayout_21;
    QFormLayout *color_formats_layout;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *add_color_format;
    QPushButton *remove_color_format;
    QFrame *line_4;
    QFormLayout *formLayout_3;
    QLabel *label_23;
    QComboBox *sceneGeometry;
    QLabel *label_42;
    QCheckBox *enableResize;
    QLabel *label_43;
    QComboBox *resolution;
    QLabel *label_44;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_45;
    QLineEdit *extentX;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_46;
    QLineEdit *extentY;
    QLabel *label_47;
    QLineEdit *layers;
    QSpacerItem *verticalSpacer_2;
    QWidget *page_3_buffer_resource_node;
    QVBoxLayout *verticalLayout_24;
    QLabel *PassPresetsLbl_4;
    QLabel *label_66;
    QFormLayout *formLayout_5;
    QLabel *label_40;
    QLineEdit *bufferName;
    QLabel *label_50;
    QLineEdit *bufferSize;
    QLabel *label_51;
    QComboBox *bufferUsage;
    QLabel *label_52;
    QCheckBox *reflectBuffer;
    QSpacerItem *verticalSpacer_3;
    QWidget *page_4_texture_resource_node;
    QVBoxLayout *verticalLayout_25;
    QLabel *PassPresetsLbl_5;
    QLabel *label_65;
    QFormLayout *formLayout_6;
    QLabel *label_53;
    QLineEdit *textureName;
    QLabel *label_54;
    QLabel *label_55;
    QLabel *label_56;
    QLabel *label_57;
    QLineEdit *textureWidth;
    QLineEdit *textureDepth;
    QLineEdit *textureHeight;
    QLineEdit *textureLayers;
    QLabel *label_58;
    QLabel *label_59;
    QLabel *label_60;
    QLabel *label_61;
    QLabel *label_62;
    QLabel *label_63;
    QLabel *label_64;
    QCheckBox *enableMips;
    QCheckBox *isHDR;
    QComboBox *filterModeMag;
    QComboBox *filterModeMin;
    QComboBox *wrapMode;
    QComboBox *textureFormat;
    QComboBox *textureType;
    QSpacerItem *verticalSpacer_4;

    void setupUi(QWidget *FrameGraphEditor)
    {
        if (FrameGraphEditor->objectName().isEmpty())
            FrameGraphEditor->setObjectName(QString::fromUtf8("FrameGraphEditor"));
        FrameGraphEditor->resize(1327, 1026);
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

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


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

        stackedWidget = new QStackedWidget(FrameGraphEditor);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setMaximumSize(QSize(300, 16777215));
        stackedWidget->setAutoFillBackground(true);
        stackedWidget->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        page_1_empty = new QWidget();
        page_1_empty->setObjectName(QString::fromUtf8("page_1_empty"));
        verticalLayout_20 = new QVBoxLayout(page_1_empty);
        verticalLayout_20->setObjectName(QString::fromUtf8("verticalLayout_20"));
        verticalLayout_20->setContentsMargins(0, 0, 0, 0);
        PassPresetsLbl_3 = new QLabel(page_1_empty);
        PassPresetsLbl_3->setObjectName(QString::fromUtf8("PassPresetsLbl_3"));
        PassPresetsLbl_3->setMinimumSize(QSize(0, 25));
        PassPresetsLbl_3->setMaximumSize(QSize(16777215, 50));
        PassPresetsLbl_3->setFont(font);
        PassPresetsLbl_3->setAutoFillBackground(false);
        PassPresetsLbl_3->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;\n"
"\n"
""));
        PassPresetsLbl_3->setScaledContents(false);
        PassPresetsLbl_3->setAlignment(Qt::AlignCenter);
        PassPresetsLbl_3->setMargin(4);

        verticalLayout_20->addWidget(PassPresetsLbl_3);

        label_21 = new QLabel(page_1_empty);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Yu Gothic UI Light"));
        font4.setPointSize(16);
        font4.setItalic(true);
        label_21->setFont(font4);
        label_21->setAlignment(Qt::AlignCenter);
        label_21->setWordWrap(true);

        verticalLayout_20->addWidget(label_21);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_20->addItem(verticalSpacer);

        stackedWidget->addWidget(page_1_empty);
        page_2_pass_node = new QWidget();
        page_2_pass_node->setObjectName(QString::fromUtf8("page_2_pass_node"));
        verticalLayout_23 = new QVBoxLayout(page_2_pass_node);
        verticalLayout_23->setObjectName(QString::fromUtf8("verticalLayout_23"));
        verticalLayout_23->setContentsMargins(0, 0, 0, 0);
        PassPresetsLbl_2 = new QLabel(page_2_pass_node);
        PassPresetsLbl_2->setObjectName(QString::fromUtf8("PassPresetsLbl_2"));
        PassPresetsLbl_2->setMinimumSize(QSize(0, 25));
        PassPresetsLbl_2->setMaximumSize(QSize(16777215, 50));
        PassPresetsLbl_2->setFont(font);
        PassPresetsLbl_2->setAutoFillBackground(false);
        PassPresetsLbl_2->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;\n"
"\n"
""));
        PassPresetsLbl_2->setScaledContents(false);
        PassPresetsLbl_2->setAlignment(Qt::AlignCenter);
        PassPresetsLbl_2->setMargin(4);

        verticalLayout_23->addWidget(PassPresetsLbl_2);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_49 = new QLabel(page_2_pass_node);
        label_49->setObjectName(QString::fromUtf8("label_49"));
        QFont font5;
        font5.setPointSize(12);
        font5.setBold(true);
        font5.setWeight(75);
        label_49->setFont(font5);

        horizontalLayout_9->addWidget(label_49);

        PassName = new QLineEdit(page_2_pass_node);
        PassName->setObjectName(QString::fromUtf8("PassName"));

        horizontalLayout_9->addWidget(PassName);


        verticalLayout_23->addLayout(horizontalLayout_9);

        line_6 = new QFrame(page_2_pass_node);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setFrameShadow(QFrame::Plain);
        line_6->setLineWidth(3);
        line_6->setFrameShape(QFrame::HLine);

        verticalLayout_23->addWidget(line_6);

        label_48 = new QLabel(page_2_pass_node);
        label_48->setObjectName(QString::fromUtf8("label_48"));
        label_48->setFont(font5);

        verticalLayout_23->addWidget(label_48);

        verticalLayout_22 = new QVBoxLayout();
        verticalLayout_22->setObjectName(QString::fromUtf8("verticalLayout_22"));
        ip_pins_layout = new QFormLayout();
        ip_pins_layout->setObjectName(QString::fromUtf8("ip_pins_layout"));
        ip_pins_layout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        add_ip_pin = new QPushButton(page_2_pass_node);
        add_ip_pin->setObjectName(QString::fromUtf8("add_ip_pin"));
        add_ip_pin->setMinimumSize(QSize(100, 0));
        add_ip_pin->setMaximumSize(QSize(100, 16777215));

        ip_pins_layout->setWidget(0, QFormLayout::LabelRole, add_ip_pin);

        remove_ip_pin = new QPushButton(page_2_pass_node);
        remove_ip_pin->setObjectName(QString::fromUtf8("remove_ip_pin"));
        remove_ip_pin->setMinimumSize(QSize(100, 0));
        remove_ip_pin->setMaximumSize(QSize(100, 16777215));

        ip_pins_layout->setWidget(0, QFormLayout::FieldRole, remove_ip_pin);


        verticalLayout_22->addLayout(ip_pins_layout);

        op_pins_layout = new QFormLayout();
        op_pins_layout->setObjectName(QString::fromUtf8("op_pins_layout"));
        op_pins_layout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        add_op_pin = new QPushButton(page_2_pass_node);
        add_op_pin->setObjectName(QString::fromUtf8("add_op_pin"));
        add_op_pin->setMinimumSize(QSize(100, 0));
        add_op_pin->setMaximumSize(QSize(100, 16777215));

        op_pins_layout->setWidget(0, QFormLayout::LabelRole, add_op_pin);

        remove_op_pin = new QPushButton(page_2_pass_node);
        remove_op_pin->setObjectName(QString::fromUtf8("remove_op_pin"));
        remove_op_pin->setMinimumSize(QSize(100, 0));
        remove_op_pin->setMaximumSize(QSize(100, 16777215));

        op_pins_layout->setWidget(0, QFormLayout::FieldRole, remove_op_pin);


        verticalLayout_22->addLayout(op_pins_layout);


        verticalLayout_23->addLayout(verticalLayout_22);

        line_5 = new QFrame(page_2_pass_node);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setFrameShadow(QFrame::Plain);
        line_5->setLineWidth(4);
        line_5->setFrameShape(QFrame::HLine);

        verticalLayout_23->addWidget(line_5);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        formLayout->setFormAlignment(Qt::AlignCenter);
        formLayout->setHorizontalSpacing(0);
        formLayout->setVerticalSpacing(0);
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        shaderFileLocation = new QLineEdit(page_2_pass_node);
        shaderFileLocation->setObjectName(QString::fromUtf8("shaderFileLocation"));
        shaderFileLocation->setReadOnly(true);

        horizontalLayout_8->addWidget(shaderFileLocation);

        browseShaderFIleBtn = new QPushButton(page_2_pass_node);
        browseShaderFIleBtn->setObjectName(QString::fromUtf8("browseShaderFIleBtn"));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/rzeditor/Browse.png"), QSize(), QIcon::Normal, QIcon::Off);
        browseShaderFIleBtn->setIcon(icon16);
        browseShaderFIleBtn->setIconSize(QSize(32, 32));
        browseShaderFIleBtn->setFlat(true);

        horizontalLayout_8->addWidget(browseShaderFIleBtn);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout_8);

        label_22 = new QLabel(page_2_pass_node);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setFont(font5);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_22);


        verticalLayout_23->addLayout(formLayout);

        line_3 = new QFrame(page_2_pass_node);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setAutoFillBackground(false);
        line_3->setStyleSheet(QString::fromUtf8("line_3:{\n"
"    background-color: rgb(255, 255, 255);\n"
"	color: rgb(255, 255, 255);\n"
"    width: 5px;\n"
"    border-width: 5px;\n"
"    border-color: #ffffff;\n"
"}"));
        line_3->setFrameShadow(QFrame::Plain);
        line_3->setLineWidth(4);
        line_3->setFrameShape(QFrame::HLine);

        verticalLayout_23->addWidget(line_3);

        label_41 = new QLabel(page_2_pass_node);
        label_41->setObjectName(QString::fromUtf8("label_41"));
        QFont font6;
        font6.setPointSize(12);
        font6.setBold(true);
        font6.setItalic(false);
        font6.setWeight(75);
        label_41->setFont(font6);

        verticalLayout_23->addWidget(label_41);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_2->setRowWrapPolicy(QFormLayout::DontWrapRows);
        formLayout_2->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_24 = new QLabel(page_2_pass_node);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        QFont font7;
        font7.setPointSize(12);
        label_24->setFont(font7);
        label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_24);

        pipelineName = new QLineEdit(page_2_pass_node);
        pipelineName->setObjectName(QString::fromUtf8("pipelineName"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, pipelineName);

        label_25 = new QLabel(page_2_pass_node);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setFont(font7);
        label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_25);

        cullMode = new QComboBox(page_2_pass_node);
        cullMode->setObjectName(QString::fromUtf8("cullMode"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, cullMode);

        label_26 = new QLabel(page_2_pass_node);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setFont(font7);
        label_26->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_26);

        polygonMode = new QComboBox(page_2_pass_node);
        polygonMode->setObjectName(QString::fromUtf8("polygonMode"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, polygonMode);

        label_27 = new QLabel(page_2_pass_node);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setFont(font7);
        label_27->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_27);

        drawType = new QComboBox(page_2_pass_node);
        drawType->setObjectName(QString::fromUtf8("drawType"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, drawType);

        label_28 = new QLabel(page_2_pass_node);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setFont(font7);
        label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_28);

        enableTransparency = new QCheckBox(page_2_pass_node);
        enableTransparency->setObjectName(QString::fromUtf8("enableTransparency"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, enableTransparency);

        label_29 = new QLabel(page_2_pass_node);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setFont(font7);
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_29);

        enableDepthTest = new QCheckBox(page_2_pass_node);
        enableDepthTest->setObjectName(QString::fromUtf8("enableDepthTest"));
        enableDepthTest->setChecked(true);

        formLayout_2->setWidget(5, QFormLayout::FieldRole, enableDepthTest);

        label_30 = new QLabel(page_2_pass_node);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        label_30->setFont(font7);
        label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label_30);

        enableDepthWrite = new QCheckBox(page_2_pass_node);
        enableDepthWrite->setObjectName(QString::fromUtf8("enableDepthWrite"));
        enableDepthWrite->setChecked(true);

        formLayout_2->setWidget(6, QFormLayout::FieldRole, enableDepthWrite);

        label_38 = new QLabel(page_2_pass_node);
        label_38->setObjectName(QString::fromUtf8("label_38"));
        label_38->setFont(font7);
        label_38->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(7, QFormLayout::LabelRole, label_38);

        depthOperation = new QComboBox(page_2_pass_node);
        depthOperation->setObjectName(QString::fromUtf8("depthOperation"));

        formLayout_2->setWidget(7, QFormLayout::FieldRole, depthOperation);

        label_31 = new QLabel(page_2_pass_node);
        label_31->setObjectName(QString::fromUtf8("label_31"));
        label_31->setFont(font7);
        label_31->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(8, QFormLayout::LabelRole, label_31);

        colorSrc = new QComboBox(page_2_pass_node);
        colorSrc->setObjectName(QString::fromUtf8("colorSrc"));

        formLayout_2->setWidget(8, QFormLayout::FieldRole, colorSrc);

        label_32 = new QLabel(page_2_pass_node);
        label_32->setObjectName(QString::fromUtf8("label_32"));
        label_32->setFont(font7);
        label_32->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(9, QFormLayout::LabelRole, label_32);

        colorDst = new QComboBox(page_2_pass_node);
        colorDst->setObjectName(QString::fromUtf8("colorDst"));

        formLayout_2->setWidget(9, QFormLayout::FieldRole, colorDst);

        label_36 = new QLabel(page_2_pass_node);
        label_36->setObjectName(QString::fromUtf8("label_36"));
        label_36->setFont(font7);
        label_36->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(10, QFormLayout::LabelRole, label_36);

        colorOp = new QComboBox(page_2_pass_node);
        colorOp->setObjectName(QString::fromUtf8("colorOp"));

        formLayout_2->setWidget(10, QFormLayout::FieldRole, colorOp);

        label_33 = new QLabel(page_2_pass_node);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        label_33->setFont(font7);
        label_33->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(11, QFormLayout::LabelRole, label_33);

        alphaSrc = new QComboBox(page_2_pass_node);
        alphaSrc->setObjectName(QString::fromUtf8("alphaSrc"));

        formLayout_2->setWidget(11, QFormLayout::FieldRole, alphaSrc);

        label_34 = new QLabel(page_2_pass_node);
        label_34->setObjectName(QString::fromUtf8("label_34"));
        label_34->setFont(font7);
        label_34->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(12, QFormLayout::LabelRole, label_34);

        alphaDst = new QComboBox(page_2_pass_node);
        alphaDst->setObjectName(QString::fromUtf8("alphaDst"));

        formLayout_2->setWidget(12, QFormLayout::FieldRole, alphaDst);

        label_37 = new QLabel(page_2_pass_node);
        label_37->setObjectName(QString::fromUtf8("label_37"));
        label_37->setFont(font7);
        label_37->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(13, QFormLayout::LabelRole, label_37);

        alphaOp = new QComboBox(page_2_pass_node);
        alphaOp->setObjectName(QString::fromUtf8("alphaOp"));

        formLayout_2->setWidget(13, QFormLayout::FieldRole, alphaOp);

        label_35 = new QLabel(page_2_pass_node);
        label_35->setObjectName(QString::fromUtf8("label_35"));
        label_35->setFont(font7);
        label_35->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(14, QFormLayout::LabelRole, label_35);

        depthFormat = new QComboBox(page_2_pass_node);
        depthFormat->setObjectName(QString::fromUtf8("depthFormat"));

        formLayout_2->setWidget(14, QFormLayout::FieldRole, depthFormat);

        label_39 = new QLabel(page_2_pass_node);
        label_39->setObjectName(QString::fromUtf8("label_39"));
        label_39->setFont(font7);
        label_39->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout_2->setWidget(15, QFormLayout::LabelRole, label_39);

        verticalLayout_21 = new QVBoxLayout();
        verticalLayout_21->setObjectName(QString::fromUtf8("verticalLayout_21"));
        color_formats_layout = new QFormLayout();
        color_formats_layout->setObjectName(QString::fromUtf8("color_formats_layout"));

        verticalLayout_21->addLayout(color_formats_layout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        add_color_format = new QPushButton(page_2_pass_node);
        add_color_format->setObjectName(QString::fromUtf8("add_color_format"));

        horizontalLayout_3->addWidget(add_color_format);

        remove_color_format = new QPushButton(page_2_pass_node);
        remove_color_format->setObjectName(QString::fromUtf8("remove_color_format"));

        horizontalLayout_3->addWidget(remove_color_format);


        verticalLayout_21->addLayout(horizontalLayout_3);


        formLayout_2->setLayout(15, QFormLayout::FieldRole, verticalLayout_21);


        verticalLayout_23->addLayout(formLayout_2);

        line_4 = new QFrame(page_2_pass_node);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShadow(QFrame::Plain);
        line_4->setLineWidth(3);
        line_4->setFrameShape(QFrame::HLine);

        verticalLayout_23->addWidget(line_4);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_23 = new QLabel(page_2_pass_node);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setFont(font7);

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_23);

        sceneGeometry = new QComboBox(page_2_pass_node);
        sceneGeometry->setObjectName(QString::fromUtf8("sceneGeometry"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, sceneGeometry);

        label_42 = new QLabel(page_2_pass_node);
        label_42->setObjectName(QString::fromUtf8("label_42"));
        label_42->setFont(font7);

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_42);

        enableResize = new QCheckBox(page_2_pass_node);
        enableResize->setObjectName(QString::fromUtf8("enableResize"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, enableResize);

        label_43 = new QLabel(page_2_pass_node);
        label_43->setObjectName(QString::fromUtf8("label_43"));
        label_43->setFont(font7);

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_43);

        resolution = new QComboBox(page_2_pass_node);
        resolution->setObjectName(QString::fromUtf8("resolution"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, resolution);

        label_44 = new QLabel(page_2_pass_node);
        label_44->setObjectName(QString::fromUtf8("label_44"));
        label_44->setFont(font7);

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_44);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_45 = new QLabel(page_2_pass_node);
        label_45->setObjectName(QString::fromUtf8("label_45"));
        label_45->setMaximumSize(QSize(10, 16777215));
        label_45->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
        label_45->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(label_45);

        extentX = new QLineEdit(page_2_pass_node);
        extentX->setObjectName(QString::fromUtf8("extentX"));
        extentX->setMaximumSize(QSize(40, 16777215));
        extentX->setAutoFillBackground(false);
        extentX->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 0, 0);\n"
"border-width:2px;\n"
"border-style:solid;"));

        horizontalLayout_5->addWidget(extentX);


        horizontalLayout_7->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_46 = new QLabel(page_2_pass_node);
        label_46->setObjectName(QString::fromUtf8("label_46"));
        label_46->setMaximumSize(QSize(10, 16777215));
        label_46->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));
        label_46->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_46);

        extentY = new QLineEdit(page_2_pass_node);
        extentY->setObjectName(QString::fromUtf8("extentY"));
        extentY->setMaximumSize(QSize(40, 16777215));
        extentY->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 255, 0);\n"
"border-width:2px;\n"
"border-style:solid;"));

        horizontalLayout_6->addWidget(extentY);


        horizontalLayout_7->addLayout(horizontalLayout_6);


        formLayout_3->setLayout(3, QFormLayout::FieldRole, horizontalLayout_7);

        label_47 = new QLabel(page_2_pass_node);
        label_47->setObjectName(QString::fromUtf8("label_47"));
        label_47->setFont(font7);

        formLayout_3->setWidget(4, QFormLayout::LabelRole, label_47);

        layers = new QLineEdit(page_2_pass_node);
        layers->setObjectName(QString::fromUtf8("layers"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, layers);


        verticalLayout_23->addLayout(formLayout_3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_23->addItem(verticalSpacer_2);

        stackedWidget->addWidget(page_2_pass_node);
        page_3_buffer_resource_node = new QWidget();
        page_3_buffer_resource_node->setObjectName(QString::fromUtf8("page_3_buffer_resource_node"));
        verticalLayout_24 = new QVBoxLayout(page_3_buffer_resource_node);
        verticalLayout_24->setObjectName(QString::fromUtf8("verticalLayout_24"));
        verticalLayout_24->setContentsMargins(0, 0, 0, 0);
        PassPresetsLbl_4 = new QLabel(page_3_buffer_resource_node);
        PassPresetsLbl_4->setObjectName(QString::fromUtf8("PassPresetsLbl_4"));
        PassPresetsLbl_4->setMinimumSize(QSize(0, 25));
        PassPresetsLbl_4->setMaximumSize(QSize(16777215, 50));
        PassPresetsLbl_4->setFont(font);
        PassPresetsLbl_4->setAutoFillBackground(false);
        PassPresetsLbl_4->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;\n"
"\n"
""));
        PassPresetsLbl_4->setScaledContents(false);
        PassPresetsLbl_4->setAlignment(Qt::AlignCenter);
        PassPresetsLbl_4->setMargin(4);

        verticalLayout_24->addWidget(PassPresetsLbl_4);

        label_66 = new QLabel(page_3_buffer_resource_node);
        label_66->setObjectName(QString::fromUtf8("label_66"));
        label_66->setFont(font7);

        verticalLayout_24->addWidget(label_66);

        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        formLayout_5->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_40 = new QLabel(page_3_buffer_resource_node);
        label_40->setObjectName(QString::fromUtf8("label_40"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_40);

        bufferName = new QLineEdit(page_3_buffer_resource_node);
        bufferName->setObjectName(QString::fromUtf8("bufferName"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, bufferName);

        label_50 = new QLabel(page_3_buffer_resource_node);
        label_50->setObjectName(QString::fromUtf8("label_50"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, label_50);

        bufferSize = new QLineEdit(page_3_buffer_resource_node);
        bufferSize->setObjectName(QString::fromUtf8("bufferSize"));

        formLayout_5->setWidget(1, QFormLayout::FieldRole, bufferSize);

        label_51 = new QLabel(page_3_buffer_resource_node);
        label_51->setObjectName(QString::fromUtf8("label_51"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_51);

        bufferUsage = new QComboBox(page_3_buffer_resource_node);
        bufferUsage->setObjectName(QString::fromUtf8("bufferUsage"));

        formLayout_5->setWidget(2, QFormLayout::FieldRole, bufferUsage);

        label_52 = new QLabel(page_3_buffer_resource_node);
        label_52->setObjectName(QString::fromUtf8("label_52"));

        formLayout_5->setWidget(3, QFormLayout::LabelRole, label_52);

        reflectBuffer = new QCheckBox(page_3_buffer_resource_node);
        reflectBuffer->setObjectName(QString::fromUtf8("reflectBuffer"));
        QFont font8;
        font8.setItalic(true);
        reflectBuffer->setFont(font8);

        formLayout_5->setWidget(3, QFormLayout::FieldRole, reflectBuffer);


        verticalLayout_24->addLayout(formLayout_5);

        verticalSpacer_3 = new QSpacerItem(20, 741, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_24->addItem(verticalSpacer_3);

        stackedWidget->addWidget(page_3_buffer_resource_node);
        page_4_texture_resource_node = new QWidget();
        page_4_texture_resource_node->setObjectName(QString::fromUtf8("page_4_texture_resource_node"));
        verticalLayout_25 = new QVBoxLayout(page_4_texture_resource_node);
        verticalLayout_25->setObjectName(QString::fromUtf8("verticalLayout_25"));
        verticalLayout_25->setContentsMargins(0, 0, 0, 0);
        PassPresetsLbl_5 = new QLabel(page_4_texture_resource_node);
        PassPresetsLbl_5->setObjectName(QString::fromUtf8("PassPresetsLbl_5"));
        PassPresetsLbl_5->setMinimumSize(QSize(0, 25));
        PassPresetsLbl_5->setMaximumSize(QSize(16777215, 50));
        PassPresetsLbl_5->setFont(font);
        PassPresetsLbl_5->setAutoFillBackground(false);
        PassPresetsLbl_5->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);\n"
"color: #ffffff;\n"
"border-style:solid;\n"
"border-color:#ffffff;\n"
"border-width:1px;\n"
"\n"
""));
        PassPresetsLbl_5->setScaledContents(false);
        PassPresetsLbl_5->setAlignment(Qt::AlignCenter);
        PassPresetsLbl_5->setMargin(4);

        verticalLayout_25->addWidget(PassPresetsLbl_5);

        label_65 = new QLabel(page_4_texture_resource_node);
        label_65->setObjectName(QString::fromUtf8("label_65"));
        label_65->setFont(font7);

        verticalLayout_25->addWidget(label_65);

        formLayout_6 = new QFormLayout();
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        formLayout_6->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_53 = new QLabel(page_4_texture_resource_node);
        label_53->setObjectName(QString::fromUtf8("label_53"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_53);

        textureName = new QLineEdit(page_4_texture_resource_node);
        textureName->setObjectName(QString::fromUtf8("textureName"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, textureName);

        label_54 = new QLabel(page_4_texture_resource_node);
        label_54->setObjectName(QString::fromUtf8("label_54"));

        formLayout_6->setWidget(1, QFormLayout::LabelRole, label_54);

        label_55 = new QLabel(page_4_texture_resource_node);
        label_55->setObjectName(QString::fromUtf8("label_55"));

        formLayout_6->setWidget(2, QFormLayout::LabelRole, label_55);

        label_56 = new QLabel(page_4_texture_resource_node);
        label_56->setObjectName(QString::fromUtf8("label_56"));

        formLayout_6->setWidget(3, QFormLayout::LabelRole, label_56);

        label_57 = new QLabel(page_4_texture_resource_node);
        label_57->setObjectName(QString::fromUtf8("label_57"));

        formLayout_6->setWidget(4, QFormLayout::LabelRole, label_57);

        textureWidth = new QLineEdit(page_4_texture_resource_node);
        textureWidth->setObjectName(QString::fromUtf8("textureWidth"));

        formLayout_6->setWidget(1, QFormLayout::FieldRole, textureWidth);

        textureDepth = new QLineEdit(page_4_texture_resource_node);
        textureDepth->setObjectName(QString::fromUtf8("textureDepth"));

        formLayout_6->setWidget(3, QFormLayout::FieldRole, textureDepth);

        textureHeight = new QLineEdit(page_4_texture_resource_node);
        textureHeight->setObjectName(QString::fromUtf8("textureHeight"));

        formLayout_6->setWidget(2, QFormLayout::FieldRole, textureHeight);

        textureLayers = new QLineEdit(page_4_texture_resource_node);
        textureLayers->setObjectName(QString::fromUtf8("textureLayers"));

        formLayout_6->setWidget(4, QFormLayout::FieldRole, textureLayers);

        label_58 = new QLabel(page_4_texture_resource_node);
        label_58->setObjectName(QString::fromUtf8("label_58"));

        formLayout_6->setWidget(5, QFormLayout::LabelRole, label_58);

        label_59 = new QLabel(page_4_texture_resource_node);
        label_59->setObjectName(QString::fromUtf8("label_59"));

        formLayout_6->setWidget(6, QFormLayout::LabelRole, label_59);

        label_60 = new QLabel(page_4_texture_resource_node);
        label_60->setObjectName(QString::fromUtf8("label_60"));

        formLayout_6->setWidget(7, QFormLayout::LabelRole, label_60);

        label_61 = new QLabel(page_4_texture_resource_node);
        label_61->setObjectName(QString::fromUtf8("label_61"));

        formLayout_6->setWidget(8, QFormLayout::LabelRole, label_61);

        label_62 = new QLabel(page_4_texture_resource_node);
        label_62->setObjectName(QString::fromUtf8("label_62"));

        formLayout_6->setWidget(9, QFormLayout::LabelRole, label_62);

        label_63 = new QLabel(page_4_texture_resource_node);
        label_63->setObjectName(QString::fromUtf8("label_63"));

        formLayout_6->setWidget(10, QFormLayout::LabelRole, label_63);

        label_64 = new QLabel(page_4_texture_resource_node);
        label_64->setObjectName(QString::fromUtf8("label_64"));

        formLayout_6->setWidget(11, QFormLayout::LabelRole, label_64);

        enableMips = new QCheckBox(page_4_texture_resource_node);
        enableMips->setObjectName(QString::fromUtf8("enableMips"));

        formLayout_6->setWidget(10, QFormLayout::FieldRole, enableMips);

        isHDR = new QCheckBox(page_4_texture_resource_node);
        isHDR->setObjectName(QString::fromUtf8("isHDR"));

        formLayout_6->setWidget(11, QFormLayout::FieldRole, isHDR);

        filterModeMag = new QComboBox(page_4_texture_resource_node);
        filterModeMag->setObjectName(QString::fromUtf8("filterModeMag"));

        formLayout_6->setWidget(9, QFormLayout::FieldRole, filterModeMag);

        filterModeMin = new QComboBox(page_4_texture_resource_node);
        filterModeMin->setObjectName(QString::fromUtf8("filterModeMin"));

        formLayout_6->setWidget(8, QFormLayout::FieldRole, filterModeMin);

        wrapMode = new QComboBox(page_4_texture_resource_node);
        wrapMode->setObjectName(QString::fromUtf8("wrapMode"));

        formLayout_6->setWidget(7, QFormLayout::FieldRole, wrapMode);

        textureFormat = new QComboBox(page_4_texture_resource_node);
        textureFormat->setObjectName(QString::fromUtf8("textureFormat"));

        formLayout_6->setWidget(6, QFormLayout::FieldRole, textureFormat);

        textureType = new QComboBox(page_4_texture_resource_node);
        textureType->setObjectName(QString::fromUtf8("textureType"));

        formLayout_6->setWidget(5, QFormLayout::FieldRole, textureType);


        verticalLayout_25->addLayout(formLayout_6);

        verticalSpacer_4 = new QSpacerItem(20, 556, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_25->addItem(verticalSpacer_4);

        stackedWidget->addWidget(page_4_texture_resource_node);

        horizontalLayout->addWidget(stackedWidget);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(FrameGraphEditor);

        stackedWidget->setCurrentIndex(3);


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
        PassPresetsLbl_3->setText(QCoreApplication::translate("FrameGraphEditor", "Node Properties Inspector", nullptr));
        label_21->setText(QCoreApplication::translate("FrameGraphEditor", "Select a Node to inspect and edit properties", nullptr));
        PassPresetsLbl_2->setText(QCoreApplication::translate("FrameGraphEditor", "Node Properties Inspector", nullptr));
        label_49->setText(QCoreApplication::translate("FrameGraphEditor", "Pass Name", nullptr));
        PassName->setPlaceholderText(QCoreApplication::translate("FrameGraphEditor", "Enter the name of the Frame Graph Pass", nullptr));
        label_48->setText(QCoreApplication::translate("FrameGraphEditor", "Pins", nullptr));
        add_ip_pin->setText(QCoreApplication::translate("FrameGraphEditor", "Add i/p Pin", nullptr));
        remove_ip_pin->setText(QCoreApplication::translate("FrameGraphEditor", "remove_ip_pin", nullptr));
        add_op_pin->setText(QCoreApplication::translate("FrameGraphEditor", "Add o/p Pin", nullptr));
        remove_op_pin->setText(QCoreApplication::translate("FrameGraphEditor", "remove_o/p_pin", nullptr));
        shaderFileLocation->setPlaceholderText(QCoreApplication::translate("FrameGraphEditor", "Choose shader file", nullptr));
        browseShaderFIleBtn->setText(QString());
        label_22->setText(QCoreApplication::translate("FrameGraphEditor", "Shader", nullptr));
        label_41->setText(QCoreApplication::translate("FrameGraphEditor", "Pipeline Settings", nullptr));
        label_24->setText(QCoreApplication::translate("FrameGraphEditor", "Name", nullptr));
        pipelineName->setPlaceholderText(QCoreApplication::translate("FrameGraphEditor", "Enter the name of the Pipeline", nullptr));
        label_25->setText(QCoreApplication::translate("FrameGraphEditor", "Cull Mode", nullptr));
        label_26->setText(QCoreApplication::translate("FrameGraphEditor", "Polygon Mode", nullptr));
        label_27->setText(QCoreApplication::translate("FrameGraphEditor", "Draw Type", nullptr));
        label_28->setText(QCoreApplication::translate("FrameGraphEditor", "Enable Transparencey", nullptr));
        enableTransparency->setText(QString());
        label_29->setText(QCoreApplication::translate("FrameGraphEditor", "Enable Depth Test", nullptr));
        enableDepthTest->setText(QString());
        label_30->setText(QCoreApplication::translate("FrameGraphEditor", "Enable Depth Write", nullptr));
        enableDepthWrite->setText(QString());
        label_38->setText(QCoreApplication::translate("FrameGraphEditor", "Depth Operation", nullptr));
        label_31->setText(QCoreApplication::translate("FrameGraphEditor", "Color Src", nullptr));
        label_32->setText(QCoreApplication::translate("FrameGraphEditor", "Color Dst", nullptr));
        label_36->setText(QCoreApplication::translate("FrameGraphEditor", "Color Operation", nullptr));
        label_33->setText(QCoreApplication::translate("FrameGraphEditor", "Alpha Src", nullptr));
        label_34->setText(QCoreApplication::translate("FrameGraphEditor", "Alpha Dst", nullptr));
        label_37->setText(QCoreApplication::translate("FrameGraphEditor", "Alpha Operation", nullptr));
        label_35->setText(QCoreApplication::translate("FrameGraphEditor", "Depth Format", nullptr));
        label_39->setText(QCoreApplication::translate("FrameGraphEditor", "Color Formats", nullptr));
        add_color_format->setText(QCoreApplication::translate("FrameGraphEditor", "Add", nullptr));
        remove_color_format->setText(QCoreApplication::translate("FrameGraphEditor", "Remove", nullptr));
        label_23->setText(QCoreApplication::translate("FrameGraphEditor", "Geometry Mode", nullptr));
        label_42->setText(QCoreApplication::translate("FrameGraphEditor", "Enable Resize", nullptr));
        enableResize->setText(QString());
        label_43->setText(QCoreApplication::translate("FrameGraphEditor", "Resolution", nullptr));
        label_44->setText(QCoreApplication::translate("FrameGraphEditor", "Extents", nullptr));
        label_45->setText(QCoreApplication::translate("FrameGraphEditor", "x", nullptr));
        label_46->setText(QCoreApplication::translate("FrameGraphEditor", "y", nullptr));
        label_47->setText(QCoreApplication::translate("FrameGraphEditor", "Layers", nullptr));
        layers->setPlaceholderText(QCoreApplication::translate("FrameGraphEditor", "Enter number of rendering layers", nullptr));
        PassPresetsLbl_4->setText(QCoreApplication::translate("FrameGraphEditor", "Node Properties Inspector", nullptr));
        label_66->setText(QCoreApplication::translate("FrameGraphEditor", "Buffer Resource", nullptr));
        label_40->setText(QCoreApplication::translate("FrameGraphEditor", "Name", nullptr));
        label_50->setText(QCoreApplication::translate("FrameGraphEditor", "Size", nullptr));
        label_51->setText(QCoreApplication::translate("FrameGraphEditor", "Usage", nullptr));
        label_52->setText(QCoreApplication::translate("FrameGraphEditor", "Use shader reflection to fill data", nullptr));
        reflectBuffer->setText(QCoreApplication::translate("FrameGraphEditor", "Note: for this to work make sure the name \n"
" matches with shader variable", nullptr));
        PassPresetsLbl_5->setText(QCoreApplication::translate("FrameGraphEditor", "Node Properties Inspector", nullptr));
        label_65->setText(QCoreApplication::translate("FrameGraphEditor", "Texture Resource", nullptr));
        label_53->setText(QCoreApplication::translate("FrameGraphEditor", "Name", nullptr));
        label_54->setText(QCoreApplication::translate("FrameGraphEditor", "Width", nullptr));
        label_55->setText(QCoreApplication::translate("FrameGraphEditor", "Height", nullptr));
        label_56->setText(QCoreApplication::translate("FrameGraphEditor", "Depth", nullptr));
        label_57->setText(QCoreApplication::translate("FrameGraphEditor", "Layers", nullptr));
        label_58->setText(QCoreApplication::translate("FrameGraphEditor", "Type", nullptr));
        label_59->setText(QCoreApplication::translate("FrameGraphEditor", "Format", nullptr));
        label_60->setText(QCoreApplication::translate("FrameGraphEditor", "Wrap Mode", nullptr));
        label_61->setText(QCoreApplication::translate("FrameGraphEditor", "Filetring Mode: Min", nullptr));
        label_62->setText(QCoreApplication::translate("FrameGraphEditor", "Filetring Mode: Mag", nullptr));
        label_63->setText(QCoreApplication::translate("FrameGraphEditor", "enable Mips", nullptr));
        label_64->setText(QCoreApplication::translate("FrameGraphEditor", "is HDR", nullptr));
        enableMips->setText(QString());
        isHDR->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FrameGraphEditor: public Ui_FrameGraphEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RZEFRAMEGRAPHEDITOR_H
