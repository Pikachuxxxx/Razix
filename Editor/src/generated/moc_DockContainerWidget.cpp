/****************************************************************************
** Meta object code from reading C++ file 'DockContainerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../Extensions/QtADS/DockContainerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DockContainerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ads__CDockContainerWidget_t {
    QByteArrayData data[11];
    char stringdata0[181];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ads__CDockContainerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ads__CDockContainerWidget_t qt_meta_stringdata_ads__CDockContainerWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ads::CDockContainerWidget"
QT_MOC_LITERAL(1, 26, 14), // "dockAreasAdded"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 21), // "autoHideWidgetCreated"
QT_MOC_LITERAL(4, 64, 28), // "ads::CAutoHideDockContainer*"
QT_MOC_LITERAL(5, 93, 14), // "AutoHideWidget"
QT_MOC_LITERAL(6, 108, 16), // "dockAreasRemoved"
QT_MOC_LITERAL(7, 125, 19), // "dockAreaViewToggled"
QT_MOC_LITERAL(8, 145, 21), // "ads::CDockAreaWidget*"
QT_MOC_LITERAL(9, 167, 8), // "DockArea"
QT_MOC_LITERAL(10, 176, 4) // "Open"

    },
    "ads::CDockContainerWidget\0dockAreasAdded\0"
    "\0autoHideWidgetCreated\0"
    "ads::CAutoHideDockContainer*\0"
    "AutoHideWidget\0dockAreasRemoved\0"
    "dockAreaViewToggled\0ads::CDockAreaWidget*\0"
    "DockArea\0Open"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ads__CDockContainerWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,
       6,    0,   38,    2, 0x06 /* Public */,
       7,    2,   39,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Bool,    9,   10,

       0        // eod
};

void ads::CDockContainerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CDockContainerWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dockAreasAdded(); break;
        case 1: _t->autoHideWidgetCreated((*reinterpret_cast< ads::CAutoHideDockContainer*(*)>(_a[1]))); break;
        case 2: _t->dockAreasRemoved(); break;
        case 3: _t->dockAreaViewToggled((*reinterpret_cast< ads::CDockAreaWidget*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CDockContainerWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockContainerWidget::dockAreasAdded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CDockContainerWidget::*)(ads::CAutoHideDockContainer * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockContainerWidget::autoHideWidgetCreated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CDockContainerWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockContainerWidget::dockAreasRemoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CDockContainerWidget::*)(ads::CDockAreaWidget * , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockContainerWidget::dockAreaViewToggled)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ads::CDockContainerWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_ads__CDockContainerWidget.data,
    qt_meta_data_ads__CDockContainerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ads::CDockContainerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ads::CDockContainerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ads__CDockContainerWidget.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int ads::CDockContainerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ads::CDockContainerWidget::dockAreasAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ads::CDockContainerWidget::autoHideWidgetCreated(ads::CAutoHideDockContainer * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ads::CDockContainerWidget::dockAreasRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ads::CDockContainerWidget::dockAreaViewToggled(ads::CDockAreaWidget * _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
