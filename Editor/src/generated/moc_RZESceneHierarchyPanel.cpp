/****************************************************************************
** Meta object code from reading C++ file 'RZESceneHierarchyPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZESceneHierarchyPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZESceneHierarchyPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel_t {
    QByteArrayData data[8];
    char stringdata0[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel_t qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel = {
    {
QT_MOC_LITERAL(0, 0, 37), // "Razix::Editor::RZESceneHierar..."
QT_MOC_LITERAL(1, 38, 16), // "OnEntitySelected"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 8), // "RZEntity"
QT_MOC_LITERAL(4, 65, 6), // "entity"
QT_MOC_LITERAL(5, 72, 14), // "OnItemSelected"
QT_MOC_LITERAL(6, 87, 22), // "OnEntitySelectedByUser"
QT_MOC_LITERAL(7, 110, 11) // "UpdatePanel"

    },
    "Razix::Editor::RZESceneHierarchyPanel\0"
    "OnEntitySelected\0\0RZEntity\0entity\0"
    "OnItemSelected\0OnEntitySelectedByUser\0"
    "UpdatePanel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZESceneHierarchyPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x0a /* Public */,
       6,    1,   38,    2, 0x0a /* Public */,
       7,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZESceneHierarchyPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZESceneHierarchyPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnEntitySelected((*reinterpret_cast< RZEntity(*)>(_a[1]))); break;
        case 1: _t->OnItemSelected(); break;
        case 2: _t->OnEntitySelectedByUser((*reinterpret_cast< RZEntity(*)>(_a[1]))); break;
        case 3: _t->UpdatePanel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RZESceneHierarchyPanel::*)(RZEntity );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RZESceneHierarchyPanel::OnEntitySelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZESceneHierarchyPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel.data,
    qt_meta_data_Razix__Editor__RZESceneHierarchyPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZESceneHierarchyPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZESceneHierarchyPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZESceneHierarchyPanel.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int Razix::Editor::RZESceneHierarchyPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Razix::Editor::RZESceneHierarchyPanel::OnEntitySelected(RZEntity _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
