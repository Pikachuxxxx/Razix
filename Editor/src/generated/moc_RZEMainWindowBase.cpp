/****************************************************************************
** Meta object code from reading C++ file 'RZEMainWindowBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEMainWindowCentralWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEMainWindowBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEMainWindowBase_t {
    QByteArrayData data[1];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEMainWindowBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEMainWindowBase_t qt_meta_stringdata_Razix__Editor__RZEMainWindowBase = {
    {
QT_MOC_LITERAL(0, 0, 32) // "Razix::Editor::RZEMainWindowBase"

    },
    "Razix::Editor::RZEMainWindowBase"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEMainWindowBase[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Razix::Editor::RZEMainWindowCentralWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEMainWindowCentralWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QGoodWindow::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEMainWindowBase.data,
    qt_meta_data_Razix__Editor__RZEMainWindowBase,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEMainWindowCentralWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEMainWindowCentralWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEMainWindowBase.stringdata0))
        return static_cast<void*>(this);
    return QGoodWindow::qt_metacast(_clname);
}

int Razix::Editor::RZEMainWindowCentralWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGoodWindow::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
