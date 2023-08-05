/****************************************************************************
** Meta object code from reading C++ file 'RZEInspectorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEInspectorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEInspectorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEInspectorWindow_t {
    QByteArrayData data[16];
    char stringdata0[299];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEInspectorWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEInspectorWindow_t qt_meta_stringdata_Razix__Editor__RZEInspectorWindow = {
    {
QT_MOC_LITERAL(0, 0, 33), // "Razix::Editor::RZEInspectorWi..."
QT_MOC_LITERAL(1, 34, 24), // "InspectorPropertyChanged"
QT_MOC_LITERAL(2, 59, 0), // ""
QT_MOC_LITERAL(3, 60, 22), // "OnMeshMaterialSelected"
QT_MOC_LITERAL(4, 83, 21), // "Graphics::RZMaterial*"
QT_MOC_LITERAL(5, 105, 8), // "material"
QT_MOC_LITERAL(6, 114, 10), // "OnNameEdit"
QT_MOC_LITERAL(7, 125, 16), // "OnEntitySelected"
QT_MOC_LITERAL(8, 142, 8), // "RZEntity"
QT_MOC_LITERAL(9, 151, 6), // "entity"
QT_MOC_LITERAL(10, 158, 29), // "ShowComponentsUtilContextMenu"
QT_MOC_LITERAL(11, 188, 18), // "AddCameraComponent"
QT_MOC_LITERAL(12, 207, 17), // "AddLightComponent"
QT_MOC_LITERAL(13, 225, 21), // "AddLuaScriptComponent"
QT_MOC_LITERAL(14, 247, 24), // "AddMeshRendererComponent"
QT_MOC_LITERAL(15, 272, 26) // "AddSpriteRendererComponent"

    },
    "Razix::Editor::RZEInspectorWindow\0"
    "InspectorPropertyChanged\0\0"
    "OnMeshMaterialSelected\0Graphics::RZMaterial*\0"
    "material\0OnNameEdit\0OnEntitySelected\0"
    "RZEntity\0entity\0ShowComponentsUtilContextMenu\0"
    "AddCameraComponent\0AddLightComponent\0"
    "AddLuaScriptComponent\0AddMeshRendererComponent\0"
    "AddSpriteRendererComponent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEInspectorWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    1,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   68,    2, 0x0a /* Public */,
       7,    1,   69,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,
      12,    0,   74,    2, 0x0a /* Public */,
      13,    0,   75,    2, 0x0a /* Public */,
      14,    0,   76,    2, 0x0a /* Public */,
      15,    0,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEInspectorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEInspectorWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->InspectorPropertyChanged(); break;
        case 1: _t->OnMeshMaterialSelected((*reinterpret_cast< Graphics::RZMaterial*(*)>(_a[1]))); break;
        case 2: _t->OnNameEdit(); break;
        case 3: _t->OnEntitySelected((*reinterpret_cast< RZEntity(*)>(_a[1]))); break;
        case 4: _t->ShowComponentsUtilContextMenu(); break;
        case 5: _t->AddCameraComponent(); break;
        case 6: _t->AddLightComponent(); break;
        case 7: _t->AddLuaScriptComponent(); break;
        case 8: _t->AddMeshRendererComponent(); break;
        case 9: _t->AddSpriteRendererComponent(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RZEInspectorWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RZEInspectorWindow::InspectorPropertyChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RZEInspectorWindow::*)(Graphics::RZMaterial * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RZEInspectorWindow::OnMeshMaterialSelected)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEInspectorWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEInspectorWindow.data,
    qt_meta_data_Razix__Editor__RZEInspectorWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEInspectorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEInspectorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEInspectorWindow.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int Razix::Editor::RZEInspectorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Razix::Editor::RZEInspectorWindow::InspectorPropertyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Razix::Editor::RZEInspectorWindow::OnMeshMaterialSelected(Graphics::RZMaterial * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
