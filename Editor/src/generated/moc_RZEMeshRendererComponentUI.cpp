/****************************************************************************
** Meta object code from reading C++ file 'RZEMeshRendererComponentUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
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
    QByteArrayData data[6];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI_t qt_meta_stringdata_Razix__Editor__RZEMeshRendererComponentUI = {
    {
QT_MOC_LITERAL(0, 0, 41), // "Razix::Editor::RZEMeshRendere..."
QT_MOC_LITERAL(1, 42, 20), // "OnMeshLocationEdited"
QT_MOC_LITERAL(2, 63, 0), // ""
QT_MOC_LITERAL(3, 64, 24), // "OnMaterialLocationEdited"
QT_MOC_LITERAL(4, 89, 12), // "OnBrowseMesh"
QT_MOC_LITERAL(5, 102, 16) // "OnBrowseMaterial"

    },
    "Razix::Editor::RZEMeshRendererComponentUI\0"
    "OnMeshLocationEdited\0\0OnMaterialLocationEdited\0"
    "OnBrowseMesh\0OnBrowseMaterial"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEMeshRendererComponentUI[] = {

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

void Razix::Editor::RZEMeshRendererComponentUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEMeshRendererComponentUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnMeshLocationEdited(); break;
        case 1: _t->OnMaterialLocationEdited(); break;
        case 2: _t->OnBrowseMesh(); break;
        case 3: _t->OnBrowseMaterial(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
