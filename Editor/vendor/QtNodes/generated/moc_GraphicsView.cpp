/****************************************************************************
** Meta object code from reading C++ file 'GraphicsView.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/QtNodes/internal/GraphicsView.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphicsView.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes__GraphicsView_t {
    QByteArrayData data[11];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes__GraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes__GraphicsView_t qt_meta_stringdata_QtNodes__GraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 21), // "QtNodes::GraphicsView"
QT_MOC_LITERAL(1, 22, 12), // "scaleChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 5), // "scale"
QT_MOC_LITERAL(4, 42, 7), // "scaleUp"
QT_MOC_LITERAL(5, 50, 9), // "scaleDown"
QT_MOC_LITERAL(6, 60, 10), // "setupScale"
QT_MOC_LITERAL(7, 71, 23), // "onDeleteSelectedObjects"
QT_MOC_LITERAL(8, 95, 26), // "onDuplicateSelectedObjects"
QT_MOC_LITERAL(9, 122, 21), // "onCopySelectedObjects"
QT_MOC_LITERAL(10, 144, 14) // "onPasteObjects"

    },
    "QtNodes::GraphicsView\0scaleChanged\0\0"
    "scale\0scaleUp\0scaleDown\0setupScale\0"
    "onDeleteSelectedObjects\0"
    "onDuplicateSelectedObjects\0"
    "onCopySelectedObjects\0onPasteObjects"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes__GraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   57,    2, 0x0a /* Public */,
       5,    0,   58,    2, 0x0a /* Public */,
       6,    1,   59,    2, 0x0a /* Public */,
       7,    0,   62,    2, 0x0a /* Public */,
       8,    0,   63,    2, 0x0a /* Public */,
       9,    0,   64,    2, 0x0a /* Public */,
      10,    0,   65,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QtNodes::GraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GraphicsView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scaleChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->scaleUp(); break;
        case 2: _t->scaleDown(); break;
        case 3: _t->setupScale((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->onDeleteSelectedObjects(); break;
        case 5: _t->onDuplicateSelectedObjects(); break;
        case 6: _t->onCopySelectedObjects(); break;
        case 7: _t->onPasteObjects(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GraphicsView::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::scaleChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtNodes::GraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_QtNodes__GraphicsView.data,
    qt_meta_data_QtNodes__GraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtNodes::GraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::GraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtNodes__GraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int QtNodes::GraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
void QtNodes::GraphicsView::scaleChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
