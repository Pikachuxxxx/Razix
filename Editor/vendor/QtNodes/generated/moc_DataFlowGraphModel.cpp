/****************************************************************************
** Meta object code from reading C++ file 'DataFlowGraphModel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/QtNodes/internal/DataFlowGraphModel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataFlowGraphModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__DataFlowGraphModel_t {
    QByteArrayData data[10];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes__DataFlowGraphModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes__DataFlowGraphModel_t qt_meta_stringdata_QtNodes__DataFlowGraphModel = {
    {
QT_MOC_LITERAL(0, 0, 27), // "QtNodes::DataFlowGraphModel"
QT_MOC_LITERAL(1, 28, 16), // "inPortDataWasSet"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 6), // "NodeId"
QT_MOC_LITERAL(4, 53, 8), // "PortType"
QT_MOC_LITERAL(5, 62, 9), // "PortIndex"
QT_MOC_LITERAL(6, 72, 20), // "onOutPortDataUpdated"
QT_MOC_LITERAL(7, 93, 6), // "nodeId"
QT_MOC_LITERAL(8, 100, 9), // "portIndex"
QT_MOC_LITERAL(9, 110, 20) // "propagateEmptyDataTo"

    },
    "QtNodes::DataFlowGraphModel\0"
    "inPortDataWasSet\0\0NodeId\0PortType\0"
    "PortIndex\0onOutPortDataUpdated\0nodeId\0"
    "portIndex\0propagateEmptyDataTo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__DataFlowGraphModel[] = {

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
       1,    3,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   36,    2, 0x08 /* Private */,
       9,    2,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4, 0x80000000 | 5,    2,    2,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    7,    8,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    7,    8,

       0        // eod
};

void QtNodes::DataFlowGraphModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataFlowGraphModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->inPortDataWasSet((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortType(*)>(_a[2])),(*reinterpret_cast< const PortIndex(*)>(_a[3]))); break;
        case 1: _t->onOutPortDataUpdated((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2]))); break;
        case 2: _t->propagateEmptyDataTo((*reinterpret_cast< const NodeId(*)>(_a[1])),(*reinterpret_cast< const PortIndex(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataFlowGraphModel::*)(NodeId const , PortType const , PortIndex const );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataFlowGraphModel::inPortDataWasSet)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtNodes::DataFlowGraphModel::staticMetaObject = { {
    QMetaObject::SuperData::link<AbstractGraphModel::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__DataFlowGraphModel.data,
    qt_meta_data_QtNodes__DataFlowGraphModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtNodes::DataFlowGraphModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::DataFlowGraphModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__DataFlowGraphModel.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Serializable"))
        return static_cast< Serializable*>(this);
    return AbstractGraphModel::qt_metacast(_clname);
}

int QtNodes::DataFlowGraphModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractGraphModel::qt_metacall(_c, _id, _a);
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
void QtNodes::DataFlowGraphModel::inPortDataWasSet(NodeId const _t1, PortType const _t2, PortIndex const _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
