/****************************************************************************
** Meta object code from reading C++ file 'DockManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../Extensions/QtADS/DockManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DockManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ads__CDockManager_t {
    QByteArrayData data[28];
    char stringdata0[482];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ads__CDockManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ads__CDockManager_t qt_meta_stringdata_ads__CDockManager = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ads::CDockManager"
QT_MOC_LITERAL(1, 18, 22), // "perspectiveListChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 21), // "perspectiveListLoaded"
QT_MOC_LITERAL(4, 64, 19), // "perspectivesRemoved"
QT_MOC_LITERAL(5, 84, 14), // "restoringState"
QT_MOC_LITERAL(6, 99, 13), // "stateRestored"
QT_MOC_LITERAL(7, 113, 18), // "openingPerspective"
QT_MOC_LITERAL(8, 132, 15), // "PerspectiveName"
QT_MOC_LITERAL(9, 148, 17), // "perspectiveOpened"
QT_MOC_LITERAL(10, 166, 21), // "floatingWidgetCreated"
QT_MOC_LITERAL(11, 188, 28), // "ads::CFloatingDockContainer*"
QT_MOC_LITERAL(12, 217, 14), // "FloatingWidget"
QT_MOC_LITERAL(13, 232, 15), // "dockAreaCreated"
QT_MOC_LITERAL(14, 248, 21), // "ads::CDockAreaWidget*"
QT_MOC_LITERAL(15, 270, 8), // "DockArea"
QT_MOC_LITERAL(16, 279, 15), // "dockWidgetAdded"
QT_MOC_LITERAL(17, 295, 17), // "ads::CDockWidget*"
QT_MOC_LITERAL(18, 313, 10), // "DockWidget"
QT_MOC_LITERAL(19, 324, 26), // "dockWidgetAboutToBeRemoved"
QT_MOC_LITERAL(20, 351, 17), // "dockWidgetRemoved"
QT_MOC_LITERAL(21, 369, 24), // "focusedDockWidgetChanged"
QT_MOC_LITERAL(22, 394, 3), // "old"
QT_MOC_LITERAL(23, 398, 3), // "now"
QT_MOC_LITERAL(24, 402, 15), // "openPerspective"
QT_MOC_LITERAL(25, 418, 20), // "setDockWidgetFocused"
QT_MOC_LITERAL(26, 439, 12), // "CDockWidget*"
QT_MOC_LITERAL(27, 452, 29) // "hideManagerAndFloatingWidgets"

    },
    "ads::CDockManager\0perspectiveListChanged\0"
    "\0perspectiveListLoaded\0perspectivesRemoved\0"
    "restoringState\0stateRestored\0"
    "openingPerspective\0PerspectiveName\0"
    "perspectiveOpened\0floatingWidgetCreated\0"
    "ads::CFloatingDockContainer*\0"
    "FloatingWidget\0dockAreaCreated\0"
    "ads::CDockAreaWidget*\0DockArea\0"
    "dockWidgetAdded\0ads::CDockWidget*\0"
    "DockWidget\0dockWidgetAboutToBeRemoved\0"
    "dockWidgetRemoved\0focusedDockWidgetChanged\0"
    "old\0now\0openPerspective\0setDockWidgetFocused\0"
    "CDockWidget*\0hideManagerAndFloatingWidgets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ads__CDockManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,
       6,    0,   98,    2, 0x06 /* Public */,
       7,    1,   99,    2, 0x06 /* Public */,
       9,    1,  102,    2, 0x06 /* Public */,
      10,    1,  105,    2, 0x06 /* Public */,
      13,    1,  108,    2, 0x06 /* Public */,
      16,    1,  111,    2, 0x06 /* Public */,
      19,    1,  114,    2, 0x06 /* Public */,
      20,    1,  117,    2, 0x06 /* Public */,
      21,    2,  120,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      24,    1,  125,    2, 0x0a /* Public */,
      25,    1,  128,    2, 0x0a /* Public */,
      27,    0,  131,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 17,   22,   23,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 26,   18,
    QMetaType::Void,

       0        // eod
};

void ads::CDockManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CDockManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->perspectiveListChanged(); break;
        case 1: _t->perspectiveListLoaded(); break;
        case 2: _t->perspectivesRemoved(); break;
        case 3: _t->restoringState(); break;
        case 4: _t->stateRestored(); break;
        case 5: _t->openingPerspective((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->perspectiveOpened((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->floatingWidgetCreated((*reinterpret_cast< ads::CFloatingDockContainer*(*)>(_a[1]))); break;
        case 8: _t->dockAreaCreated((*reinterpret_cast< ads::CDockAreaWidget*(*)>(_a[1]))); break;
        case 9: _t->dockWidgetAdded((*reinterpret_cast< ads::CDockWidget*(*)>(_a[1]))); break;
        case 10: _t->dockWidgetAboutToBeRemoved((*reinterpret_cast< ads::CDockWidget*(*)>(_a[1]))); break;
        case 11: _t->dockWidgetRemoved((*reinterpret_cast< ads::CDockWidget*(*)>(_a[1]))); break;
        case 12: _t->focusedDockWidgetChanged((*reinterpret_cast< ads::CDockWidget*(*)>(_a[1])),(*reinterpret_cast< ads::CDockWidget*(*)>(_a[2]))); break;
        case 13: _t->openPerspective((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->setDockWidgetFocused((*reinterpret_cast< CDockWidget*(*)>(_a[1]))); break;
        case 15: _t->hideManagerAndFloatingWidgets(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ads::CFloatingDockContainer* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ads::CDockWidget* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ads::CDockWidget* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ads::CDockWidget* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ads::CDockWidget* >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CDockWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CDockManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::perspectiveListChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::perspectiveListLoaded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::perspectivesRemoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::restoringState)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::stateRestored)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::openingPerspective)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::perspectiveOpened)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CFloatingDockContainer * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::floatingWidgetCreated)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CDockAreaWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::dockAreaCreated)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CDockWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::dockWidgetAdded)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CDockWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::dockWidgetAboutToBeRemoved)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CDockWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::dockWidgetRemoved)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (CDockManager::*)(ads::CDockWidget * , ads::CDockWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CDockManager::focusedDockWidgetChanged)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ads::CDockManager::staticMetaObject = { {
    QMetaObject::SuperData::link<CDockContainerWidget::staticMetaObject>(),
    qt_meta_stringdata_ads__CDockManager.data,
    qt_meta_data_ads__CDockManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ads::CDockManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ads::CDockManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ads__CDockManager.stringdata0))
        return static_cast<void*>(this);
    return CDockContainerWidget::qt_metacast(_clname);
}

int ads::CDockManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CDockContainerWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ads::CDockManager::perspectiveListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ads::CDockManager::perspectiveListLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ads::CDockManager::perspectivesRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ads::CDockManager::restoringState()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ads::CDockManager::stateRestored()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ads::CDockManager::openingPerspective(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ads::CDockManager::perspectiveOpened(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ads::CDockManager::floatingWidgetCreated(ads::CFloatingDockContainer * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ads::CDockManager::dockAreaCreated(ads::CDockAreaWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ads::CDockManager::dockWidgetAdded(ads::CDockWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ads::CDockManager::dockWidgetAboutToBeRemoved(ads::CDockWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ads::CDockManager::dockWidgetRemoved(ads::CDockWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ads::CDockManager::focusedDockWidgetChanged(ads::CDockWidget * _t1, ads::CDockWidget * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
