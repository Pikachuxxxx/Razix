/****************************************************************************
** Meta object code from reading C++ file 'RZEProjectBrowser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/RZEProjectBrowser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEProjectBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEProjectBrowser_t {
    QByteArrayData data[10];
    char stringdata0[167];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEProjectBrowser_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEProjectBrowser_t qt_meta_stringdata_Razix__Editor__RZEProjectBrowser = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Razix::Editor::RZEProjectBrowser"
QT_MOC_LITERAL(1, 33, 16), // "on_SelectProject"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 68, 4), // "item"
QT_MOC_LITERAL(5, 73, 20), // "on_NewProjectPressed"
QT_MOC_LITERAL(6, 94, 21), // "on_OpenProjectPressed"
QT_MOC_LITERAL(7, 116, 16), // "on_BrowsePressed"
QT_MOC_LITERAL(8, 133, 16), // "on_CreatePressed"
QT_MOC_LITERAL(9, 150, 16) // "on_CancelPressed"

    },
    "Razix::Editor::RZEProjectBrowser\0"
    "on_SelectProject\0\0QListWidgetItem*\0"
    "item\0on_NewProjectPressed\0"
    "on_OpenProjectPressed\0on_BrowsePressed\0"
    "on_CreatePressed\0on_CancelPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEProjectBrowser[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    0,   49,    2, 0x0a /* Public */,
       8,    0,   50,    2, 0x0a /* Public */,
       9,    0,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEProjectBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEProjectBrowser *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_SelectProject((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->on_NewProjectPressed(); break;
        case 2: _t->on_OpenProjectPressed(); break;
        case 3: _t->on_BrowsePressed(); break;
        case 4: _t->on_CreatePressed(); break;
        case 5: _t->on_CancelPressed(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEProjectBrowser::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEProjectBrowser.data,
    qt_meta_data_Razix__Editor__RZEProjectBrowser,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEProjectBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEProjectBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEProjectBrowser.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Razix::Editor::RZEProjectBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
