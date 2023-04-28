/****************************************************************************
** Meta object code from reading C++ file 'RZETransformComponentUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Widgets/ComponentsUI/RZETransformComponentUI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZETransformComponentUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZETransformComponentUI_t {
    QByteArrayData data[11];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZETransformComponentUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZETransformComponentUI_t qt_meta_stringdata_Razix__Editor__RZETransformComponentUI = {
    {
QT_MOC_LITERAL(0, 0, 38), // "Razix::Editor::RZETransformCo..."
QT_MOC_LITERAL(1, 39, 12), // "OnPosXEdited"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 12), // "OnPosYEdited"
QT_MOC_LITERAL(4, 66, 12), // "OnPosZEdited"
QT_MOC_LITERAL(5, 79, 12), // "OnRotXEdited"
QT_MOC_LITERAL(6, 92, 12), // "OnRotYEdited"
QT_MOC_LITERAL(7, 105, 12), // "OnRotZEdited"
QT_MOC_LITERAL(8, 118, 14), // "OnScaleXEdited"
QT_MOC_LITERAL(9, 133, 14), // "OnScaleYEdited"
QT_MOC_LITERAL(10, 148, 14) // "OnScaleZEdited"

    },
    "Razix::Editor::RZETransformComponentUI\0"
    "OnPosXEdited\0\0OnPosYEdited\0OnPosZEdited\0"
    "OnRotXEdited\0OnRotYEdited\0OnRotZEdited\0"
    "OnScaleXEdited\0OnScaleYEdited\0"
    "OnScaleZEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZETransformComponentUI[] = {

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
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    0,   66,    2, 0x0a /* Public */,
      10,    0,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZETransformComponentUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZETransformComponentUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnPosXEdited(); break;
        case 1: _t->OnPosYEdited(); break;
        case 2: _t->OnPosZEdited(); break;
        case 3: _t->OnRotXEdited(); break;
        case 4: _t->OnRotYEdited(); break;
        case 5: _t->OnRotZEdited(); break;
        case 6: _t->OnScaleXEdited(); break;
        case 7: _t->OnScaleYEdited(); break;
        case 8: _t->OnScaleZEdited(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZETransformComponentUI::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZETransformComponentUI.data,
    qt_meta_data_Razix__Editor__RZETransformComponentUI,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZETransformComponentUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZETransformComponentUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZETransformComponentUI.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZETransformComponentUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
