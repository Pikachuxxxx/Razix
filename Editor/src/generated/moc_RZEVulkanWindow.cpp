/****************************************************************************
** Meta object code from reading C++ file 'RZEVulkanWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEVulkanWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEVulkanWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEVulkanWindow_t {
    QByteArrayData data[5];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEVulkanWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEVulkanWindow_t qt_meta_stringdata_Razix__Editor__RZEVulkanWindow = {
    {
QT_MOC_LITERAL(0, 0, 30), // "Razix::Editor::RZEVulkanWindow"
QT_MOC_LITERAL(1, 31, 16), // "OnEntitySelected"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 8), // "RZEntity"
QT_MOC_LITERAL(4, 58, 6) // "entity"

    },
    "Razix::Editor::RZEVulkanWindow\0"
    "OnEntitySelected\0\0RZEntity\0entity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEVulkanWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void Razix::Editor::RZEVulkanWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEVulkanWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnEntitySelected((*reinterpret_cast< RZEntity(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RZEVulkanWindow::*)(RZEntity );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RZEVulkanWindow::OnEntitySelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEVulkanWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWindow::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEVulkanWindow.data,
    qt_meta_data_Razix__Editor__RZEVulkanWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEVulkanWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEVulkanWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEVulkanWindow.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Razix::RZInput"))
        return static_cast< Razix::RZInput*>(this);
    return QWindow::qt_metacast(_clname);
}

int Razix::Editor::RZEVulkanWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Razix::Editor::RZEVulkanWindow::OnEntitySelected(RZEntity _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
