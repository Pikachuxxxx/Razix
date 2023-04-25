/****************************************************************************
** Meta object code from reading C++ file 'DataFlowGraphicsScene.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/QtNodes/internal/DataFlowGraphicsScene.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataFlowGraphicsScene.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t {
    QByteArrayData data[5];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes__DataFlowGraphicsScene_t qt_meta_stringdata_QtNodes__DataFlowGraphicsScene = {
    {
QT_MOC_LITERAL(0, 0, 30), // "QtNodes::DataFlowGraphicsScene"
QT_MOC_LITERAL(1, 31, 11), // "sceneLoaded"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 4), // "save"
QT_MOC_LITERAL(4, 49, 4) // "load"

    },
    "QtNodes::DataFlowGraphicsScene\0"
    "sceneLoaded\0\0save\0load"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__DataFlowGraphicsScene[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   30,    2, 0x0a /* Public */,
       4,    0,   31,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QtNodes::DataFlowGraphicsScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataFlowGraphicsScene *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sceneLoaded(); break;
        case 1: _t->save(); break;
        case 2: _t->load(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataFlowGraphicsScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataFlowGraphicsScene::sceneLoaded)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject QtNodes::DataFlowGraphicsScene::staticMetaObject = { {
    QMetaObject::SuperData::link<BasicGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__DataFlowGraphicsScene.data,
    qt_meta_data_QtNodes__DataFlowGraphicsScene,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtNodes::DataFlowGraphicsScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::DataFlowGraphicsScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__DataFlowGraphicsScene.stringdata0))
        return static_cast<void*>(this);
    return BasicGraphicsScene::qt_metacast(_clname);
}

int QtNodes::DataFlowGraphicsScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BasicGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QtNodes::DataFlowGraphicsScene::sceneLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
