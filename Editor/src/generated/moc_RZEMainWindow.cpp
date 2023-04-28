/****************************************************************************
** Meta object code from reading C++ file 'RZEMainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEMainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEMainWindow_t {
    QByteArrayData data[17];
    char stringdata0[274];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEMainWindow_t qt_meta_stringdata_Razix__Editor__RZEMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Razix::Editor::RZEMainWindow"
QT_MOC_LITERAL(1, 29, 20), // "OnEntityAddedToScene"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 21), // "on_SaveProjectPressed"
QT_MOC_LITERAL(4, 73, 21), // "on_OpenProjectPressed"
QT_MOC_LITERAL(5, 95, 20), // "on_NewProjectPressed"
QT_MOC_LITERAL(6, 116, 12), // "on_SaveScene"
QT_MOC_LITERAL(7, 129, 12), // "on_LoadScene"
QT_MOC_LITERAL(8, 142, 11), // "on_NewScene"
QT_MOC_LITERAL(9, 154, 19), // "on_RenderAPIChanged"
QT_MOC_LITERAL(10, 174, 5), // "index"
QT_MOC_LITERAL(11, 180, 19), // "set_TranslateGuizmo"
QT_MOC_LITERAL(12, 200, 16), // "set_RotateGuizmo"
QT_MOC_LITERAL(13, 217, 15), // "set_ScaleGuizmo"
QT_MOC_LITERAL(14, 233, 17), // "toggle_WorldLocal"
QT_MOC_LITERAL(15, 251, 15), // "toggle_GridSnap"
QT_MOC_LITERAL(16, 267, 6) // "update"

    },
    "Razix::Editor::RZEMainWindow\0"
    "OnEntityAddedToScene\0\0on_SaveProjectPressed\0"
    "on_OpenProjectPressed\0on_NewProjectPressed\0"
    "on_SaveScene\0on_LoadScene\0on_NewScene\0"
    "on_RenderAPIChanged\0index\0set_TranslateGuizmo\0"
    "set_RotateGuizmo\0set_ScaleGuizmo\0"
    "toggle_WorldLocal\0toggle_GridSnap\0"
    "update"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    0,   90,    2, 0x0a /* Public */,
       9,    1,   91,    2, 0x0a /* Public */,
      11,    0,   94,    2, 0x0a /* Public */,
      12,    0,   95,    2, 0x0a /* Public */,
      13,    0,   96,    2, 0x0a /* Public */,
      14,    0,   97,    2, 0x0a /* Public */,
      15,    0,   98,    2, 0x0a /* Public */,
      16,    0,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZEMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnEntityAddedToScene(); break;
        case 1: _t->on_SaveProjectPressed(); break;
        case 2: _t->on_OpenProjectPressed(); break;
        case 3: _t->on_NewProjectPressed(); break;
        case 4: _t->on_SaveScene(); break;
        case 5: _t->on_LoadScene(); break;
        case 6: _t->on_NewScene(); break;
        case 7: _t->on_RenderAPIChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->set_TranslateGuizmo(); break;
        case 9: _t->set_RotateGuizmo(); break;
        case 10: _t->set_ScaleGuizmo(); break;
        case 11: _t->toggle_WorldLocal(); break;
        case 12: _t->toggle_GridSnap(); break;
        case 13: _t->update(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RZEMainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RZEMainWindow::OnEntityAddedToScene)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEMainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEMainWindow.data,
    qt_meta_data_Razix__Editor__RZEMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Razix::Editor::RZEMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Razix::Editor::RZEMainWindow::OnEntityAddedToScene()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
