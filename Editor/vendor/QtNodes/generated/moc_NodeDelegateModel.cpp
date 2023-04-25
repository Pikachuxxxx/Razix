/****************************************************************************
** Meta object code from reading C++ file 'NodeDelegateModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/QtNodes/internal/NodeDelegateModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NodeDelegateModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__NodeDelegateModel_t {
    QByteArrayData data[22];
    char stringdata0[341];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes__NodeDelegateModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes__NodeDelegateModel_t qt_meta_stringdata_QtNodes__NodeDelegateModel = {
    {
QT_MOC_LITERAL(0, 0, 26), // "QtNodes::NodeDelegateModel"
QT_MOC_LITERAL(1, 27, 11), // "dataUpdated"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 9), // "PortIndex"
QT_MOC_LITERAL(4, 50, 5), // "index"
QT_MOC_LITERAL(5, 56, 15), // "dataInvalidated"
QT_MOC_LITERAL(6, 72, 16), // "computingStarted"
QT_MOC_LITERAL(7, 89, 17), // "computingFinished"
QT_MOC_LITERAL(8, 107, 25), // "embeddedWidgetSizeUpdated"
QT_MOC_LITERAL(9, 133, 21), // "portsAboutToBeDeleted"
QT_MOC_LITERAL(10, 155, 8), // "PortType"
QT_MOC_LITERAL(11, 164, 8), // "portType"
QT_MOC_LITERAL(12, 173, 5), // "first"
QT_MOC_LITERAL(13, 179, 4), // "last"
QT_MOC_LITERAL(14, 184, 12), // "portsDeleted"
QT_MOC_LITERAL(15, 197, 22), // "portsAboutToBeInserted"
QT_MOC_LITERAL(16, 220, 13), // "portsInserted"
QT_MOC_LITERAL(17, 234, 22), // "inputConnectionCreated"
QT_MOC_LITERAL(18, 257, 12), // "ConnectionId"
QT_MOC_LITERAL(19, 270, 22), // "inputConnectionDeleted"
QT_MOC_LITERAL(20, 293, 23), // "outputConnectionCreated"
QT_MOC_LITERAL(21, 317, 23) // "outputConnectionDeleted"

    },
    "QtNodes::NodeDelegateModel\0dataUpdated\0"
    "\0PortIndex\0index\0dataInvalidated\0"
    "computingStarted\0computingFinished\0"
    "embeddedWidgetSizeUpdated\0"
    "portsAboutToBeDeleted\0PortType\0portType\0"
    "first\0last\0portsDeleted\0portsAboutToBeInserted\0"
    "portsInserted\0inputConnectionCreated\0"
    "ConnectionId\0inputConnectionDeleted\0"
    "outputConnectionCreated\0outputConnectionDeleted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__NodeDelegateModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       5,    1,   82,    2, 0x06 /* Public */,
       6,    0,   85,    2, 0x06 /* Public */,
       7,    0,   86,    2, 0x06 /* Public */,
       8,    0,   87,    2, 0x06 /* Public */,
       9,    3,   88,    2, 0x06 /* Public */,
      14,    0,   95,    2, 0x06 /* Public */,
      15,    3,   96,    2, 0x06 /* Public */,
      16,    0,  103,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    1,  104,    2, 0x0a /* Public */,
      19,    1,  107,    2, 0x0a /* Public */,
      20,    1,  110,    2, 0x0a /* Public */,
      21,    1,  113,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 3, 0x80000000 | 3,   11,   12,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 3, 0x80000000 | 3,   11,   12,   13,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, 0x80000000 | 18,    2,

       0        // eod
};

void QtNodes::NodeDelegateModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NodeDelegateModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataUpdated((*reinterpret_cast< const PortIndex(*)>(_a[1]))); break;
        case 1: _t->dataInvalidated((*reinterpret_cast< const PortIndex(*)>(_a[1]))); break;
        case 2: _t->computingStarted(); break;
        case 3: _t->computingFinished(); break;
        case 4: _t->embeddedWidgetSizeUpdated(); break;
        case 5: _t->portsAboutToBeDeleted((*reinterpret_cast< const PortType(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2])),(*reinterpret_cast< const PortIndex(*)>(_a[3]))); break;
        case 6: _t->portsDeleted(); break;
        case 7: _t->portsAboutToBeInserted((*reinterpret_cast< const PortType(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2])),(*reinterpret_cast< const PortIndex(*)>(_a[3]))); break;
        case 8: _t->portsInserted(); break;
        case 9: _t->inputConnectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 10: _t->inputConnectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 11: _t->outputConnectionCreated((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        case 12: _t->outputConnectionDeleted((*reinterpret_cast< const ConnectionId(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NodeDelegateModel::*)(PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::dataUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)(PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::dataInvalidated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::computingStarted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::computingFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::embeddedWidgetSizeUpdated)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)(PortType const , PortIndex const , PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::portsAboutToBeDeleted)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::portsDeleted)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)(PortType const , PortIndex const , PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::portsAboutToBeInserted)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (NodeDelegateModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NodeDelegateModel::portsInserted)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtNodes::NodeDelegateModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__NodeDelegateModel.data,
    qt_meta_data_QtNodes__NodeDelegateModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtNodes::NodeDelegateModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::NodeDelegateModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__NodeDelegateModel.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::NodeDelegateModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QtNodes::NodeDelegateModel::dataUpdated(PortIndex const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::NodeDelegateModel::dataInvalidated(PortIndex const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::NodeDelegateModel::computingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QtNodes::NodeDelegateModel::computingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void QtNodes::NodeDelegateModel::embeddedWidgetSizeUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void QtNodes::NodeDelegateModel::portsAboutToBeDeleted(PortType const _t1, PortIndex const _t2, PortIndex const _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtNodes::NodeDelegateModel::portsDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void QtNodes::NodeDelegateModel::portsAboutToBeInserted(PortType const _t1, PortIndex const _t2, PortIndex const _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QtNodes::NodeDelegateModel::portsInserted()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
