/****************************************************************************
** Meta object code from reading C++ file 'RZEMeshRendererComponentUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Widgets/ComponentsUI/RZEMeshRendererComponentUI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEMeshRendererComponentUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI_t {
    QByteArrayData data[11];
    char stringdata0[218];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI_t qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI = {
    {
QT_MOC_LITERAL(0, 0, 41), // "Razix::Editor::RZEMeshRendere..."
QT_MOC_LITERAL(1, 42, 21), // "on_mesh_type_selected"
QT_MOC_LITERAL(2, 64, 0), // ""
QT_MOC_LITERAL(3, 65, 8), // "meshType"
QT_MOC_LITERAL(4, 74, 20), // "OnMeshLocationEdited"
QT_MOC_LITERAL(5, 95, 24), // "OnMaterialLocationEdited"
QT_MOC_LITERAL(6, 120, 26), // "OnEnableBoundingBoxPressed"
QT_MOC_LITERAL(7, 147, 23), // "OnReceiveShadowsPressed"
QT_MOC_LITERAL(8, 171, 12), // "OnBrowseMesh"
QT_MOC_LITERAL(9, 184, 16), // "OnBrowseMaterial"
QT_MOC_LITERAL(10, 201, 16) // "OnAddNewMaterial"

    },
    "Razix::Editor::RZEMeshRendererComponentUI\0"
    "on_mesh_type_selected\0\0meshType\0"
    "OnMeshLocationEdited\0OnMaterialLocationEdited\0"
    "OnEnableBoundingBoxPressed\0"
    "OnReceiveShadowsPressed\0OnBrowseMesh\0"
    "OnBrowseMaterial\0OnAddNewMaterial"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEMeshRendererComponentUI[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x0a /* Public */,
       4,    0,   57,    2, 0x0a /* Public */,
       5,    0,   58,    2, 0x0a /* Public */,
       6,    0,   59,    2, 0x0a /* Public */,
       7,    0,   60,    2, 0x0a /* Public */,
       8,    0,   61,    2, 0x0a /* Public */,
       9,    0,   62,    2, 0x0a /* Public */,
      10,    0,   63,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEMeshRendererComponentUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEMeshRendererComponentUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_mesh_type_selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->OnMeshLocationEdited(); break;
        case 2: _t->OnMaterialLocationEdited(); break;
        case 3: _t->OnEnableBoundingBoxPressed(); break;
        case 4: _t->OnReceiveShadowsPressed(); break;
        case 5: _t->OnBrowseMesh(); break;
        case 6: _t->OnBrowseMaterial(); break;
        case 7: _t->OnAddNewMaterial(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEMeshRendererComponentUI::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI.data,
    qt_meta_data_Razix__Editor__RZEMeshRendererComponentUI,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEMeshRendererComponentUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEMeshRendererComponentUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZEMeshRendererComponentUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
