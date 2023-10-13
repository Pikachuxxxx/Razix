/****************************************************************************
** Meta object code from reading C++ file 'RZEFrameGraphEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEFrameGraphEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEFrameGraphEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t {
    QByteArrayData data[6];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView_t qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 37), // "Razix::Editor::FrameGraphGrap..."
QT_MOC_LITERAL(1, 38, 13), // "OnAddPassNode"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 15), // "OnAddBufferNode"
QT_MOC_LITERAL(4, 69, 16), // "OnAddTextureNode"
QT_MOC_LITERAL(5, 86, 16) // "OnImportResource"

    },
    "Razix::Editor::FrameGraphGraphicsView\0"
    "OnAddPassNode\0\0OnAddBufferNode\0"
    "OnAddTextureNode\0OnImportResource"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__FrameGraphGraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::FrameGraphGraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FrameGraphGraphicsView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnAddPassNode(); break;
        case 1: _t->OnAddBufferNode(); break;
        case 2: _t->OnAddTextureNode(); break;
        case 3: _t->OnImportResource(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::FrameGraphGraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<NodeGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView.data,
    qt_meta_data_Razix__Editor__FrameGraphGraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::FrameGraphGraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::FrameGraphGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__FrameGraphGraphicsView.stringdata0))
        return static_cast<void*>(this);
    return NodeGraphicsView::qt_metacast(_clname);
}

int Razix::Editor::FrameGraphGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NodeGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t {
    QByteArrayData data[14];
    char stringdata0[249];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor_t qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor = {
    {
QT_MOC_LITERAL(0, 0, 34), // "Razix::Editor::RZEFrameGraphE..."
QT_MOC_LITERAL(1, 35, 27), // "OnImportPresetButtonClicked"
QT_MOC_LITERAL(2, 63, 0), // ""
QT_MOC_LITERAL(3, 64, 14), // "OnNodeSelected"
QT_MOC_LITERAL(4, 79, 5), // "Node*"
QT_MOC_LITERAL(5, 85, 4), // "node"
QT_MOC_LITERAL(6, 90, 17), // "OnNodeNameChanged"
QT_MOC_LITERAL(7, 108, 20), // "OnAddInputPinClicked"
QT_MOC_LITERAL(8, 129, 21), // "OnInputPinNameChanged"
QT_MOC_LITERAL(9, 151, 3), // "idx"
QT_MOC_LITERAL(10, 155, 23), // "OnRemoveInputPinClicked"
QT_MOC_LITERAL(11, 179, 21), // "OnAddOutputPinClicked"
QT_MOC_LITERAL(12, 201, 22), // "OnOutputPinNameChanged"
QT_MOC_LITERAL(13, 224, 24) // "OnRemoveOutputPinClicked"

    },
    "Razix::Editor::RZEFrameGraphEditor\0"
    "OnImportPresetButtonClicked\0\0"
    "OnNodeSelected\0Node*\0node\0OnNodeNameChanged\0"
    "OnAddInputPinClicked\0OnInputPinNameChanged\0"
    "idx\0OnRemoveInputPinClicked\0"
    "OnAddOutputPinClicked\0OnOutputPinNameChanged\0"
    "OnRemoveOutputPinClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEFrameGraphEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    1,   60,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    1,   65,    2, 0x0a /* Public */,
      10,    0,   68,    2, 0x0a /* Public */,
      11,    0,   69,    2, 0x0a /* Public */,
      12,    1,   70,    2, 0x0a /* Public */,
      13,    0,   73,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEFrameGraphEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEFrameGraphEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnImportPresetButtonClicked(); break;
        case 1: _t->OnNodeSelected((*reinterpret_cast< Node*(*)>(_a[1]))); break;
        case 2: _t->OnNodeNameChanged(); break;
        case 3: _t->OnAddInputPinClicked(); break;
        case 4: _t->OnInputPinNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->OnRemoveInputPinClicked(); break;
        case 6: _t->OnAddOutputPinClicked(); break;
        case 7: _t->OnOutputPinNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->OnRemoveOutputPinClicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEFrameGraphEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor.data,
    qt_meta_data_Razix__Editor__RZEFrameGraphEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEFrameGraphEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEFrameGraphEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEFrameGraphEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZEFrameGraphEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
