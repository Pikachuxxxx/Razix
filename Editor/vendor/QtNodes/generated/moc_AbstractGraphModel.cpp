/****************************************************************************
** Meta object code from reading C++ file 'AbstractGraphModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/QtNodes/internal/AbstractGraphModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AbstractGraphModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__AbstractGraphModel_t {
    QByteArrayData data[14];
    char stringdata0[189];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes__AbstractGraphModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes__AbstractGraphModel_t qt_meta_stringdata_QtNodes__AbstractGraphModel = {
    {
QT_MOC_LITERAL(0, 0, 27), // "QtNodes::AbstractGraphModel"
QT_MOC_LITERAL(1, 28, 17), // "connectionCreated"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 12), // "ConnectionId"
QT_MOC_LITERAL(4, 60, 12), // "connectionId"
QT_MOC_LITERAL(5, 73, 17), // "connectionDeleted"
QT_MOC_LITERAL(6, 91, 11), // "nodeCreated"
QT_MOC_LITERAL(7, 103, 6), // "NodeId"
QT_MOC_LITERAL(8, 110, 6), // "nodeId"
QT_MOC_LITERAL(9, 117, 11), // "nodeDeleted"
QT_MOC_LITERAL(10, 129, 11), // "nodeUpdated"
QT_MOC_LITERAL(11, 141, 16), // "nodeFlagsUpdated"
QT_MOC_LITERAL(12, 158, 19), // "nodePositionUpdated"
QT_MOC_LITERAL(13, 178, 10) // "modelReset"

    },
    "QtNodes::AbstractGraphModel\0"
    "connectionCreated\0\0ConnectionId\0"
    "connectionId\0connectionDeleted\0"
    "nodeCreated\0NodeId\0nodeId\0nodeDeleted\0"
    "nodeUpdated\0nodeFlagsUpdated\0"
    "nodePositionUpdated\0modelReset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__AbstractGraphModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       6,    1,   60,    2, 0x06 /* Public */,
       9,    1,   63,    2, 0x06 /* Public */,
      10,    1,   66,    2, 0x06 /* Public */,
      11,    1,   69,    2, 0x06 /* Public */,
      12,    1,   72,    2, 0x06 /* Public */,
      13,    0,   75,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,

       0        // eod
};

void QtNodes::AbstractGraphModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AbstractGraphModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 1: _t->connectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 2: _t->nodeCreated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 3: _t->nodeDeleted((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 4: _t->nodeUpdated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 5: _t->nodeFlagsUpdated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 6: _t->nodePositionUpdated((*reinterpret_cast< const NodeId(*)>(_a[1]))); break;
        case 7: _t->modelReset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AbstractGraphModel::*)(ConnectionId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::connectionCreated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(ConnectionId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::connectionDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::nodeCreated)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::nodeDeleted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::nodeUpdated)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::nodeFlagsUpdated)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)(NodeId const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::nodePositionUpdated)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (AbstractGraphModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AbstractGraphModel::modelReset)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtNodes::AbstractGraphModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__AbstractGraphModel.data,
    qt_meta_data_QtNodes__AbstractGraphModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtNodes::AbstractGraphModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::AbstractGraphModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__AbstractGraphModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::AbstractGraphModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void QtNodes::AbstractGraphModel::connectionCreated(ConnectionId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::AbstractGraphModel::connectionDeleted(ConnectionId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::AbstractGraphModel::nodeCreated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtNodes::AbstractGraphModel::nodeDeleted(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtNodes::AbstractGraphModel::nodeUpdated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QtNodes::AbstractGraphModel::nodeFlagsUpdated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtNodes::AbstractGraphModel::nodePositionUpdated(NodeId const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QtNodes::AbstractGraphModel::modelReset()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
