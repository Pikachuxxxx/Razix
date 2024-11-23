/****************************************************************************
** Meta object code from reading C++ file 'RZEWorldSettingsWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZERendererSettingsUIWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEWorldSettingsWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow_t {
    QByteArrayData data[12];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow_t qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow = {
    {
QT_MOC_LITERAL(0, 0, 37), // "Razix::Editor::RZEWorldSettin..."
QT_MOC_LITERAL(1, 38, 22), // "On_UseProceduralSkybox"
QT_MOC_LITERAL(2, 61, 0), // ""
QT_MOC_LITERAL(3, 62, 20), // "OnToneMapModeChanged"
QT_MOC_LITERAL(4, 83, 15), // "OnAAModeChanged"
QT_MOC_LITERAL(5, 99, 29), // "OnSceneSamplingPatternChanged"
QT_MOC_LITERAL(6, 129, 17), // "OnSetFilterRadius"
QT_MOC_LITERAL(7, 147, 13), // "OnSetStrength"
QT_MOC_LITERAL(8, 161, 16), // "OnShadowsEnabled"
QT_MOC_LITERAL(9, 178, 15), // "OnSkyboxEnabled"
QT_MOC_LITERAL(10, 194, 13), // "OnSSAOEnabled"
QT_MOC_LITERAL(11, 208, 14) // "OnImGuiEnabled"

    },
    "Razix::Editor::RZEWorldSettingsWindow\0"
    "On_UseProceduralSkybox\0\0OnToneMapModeChanged\0"
    "OnAAModeChanged\0OnSceneSamplingPatternChanged\0"
    "OnSetFilterRadius\0OnSetStrength\0"
    "OnShadowsEnabled\0OnSkyboxEnabled\0"
    "OnSSAOEnabled\0OnImGuiEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEWorldSettingsWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    0,   65,    2, 0x0a /* Public */,
       4,    0,   66,    2, 0x0a /* Public */,
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    0,   70,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Razix::Editor::RZERendererSettingsUIWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZERendererSettingsUIWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->On_UseProceduralSkybox(); break;
        case 1: _t->OnToneMapModeChanged(); break;
        case 2: _t->OnAAModeChanged(); break;
        case 3: _t->OnSceneSamplingPatternChanged(); break;
        case 4: _t->OnSetFilterRadius(); break;
        case 5: _t->OnSetStrength(); break;
        case 6: _t->OnShadowsEnabled(); break;
        case 7: _t->OnSkyboxEnabled(); break;
        case 8: _t->OnSSAOEnabled(); break;
        case 9: _t->OnImGuiEnabled(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZERendererSettingsUIWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow.data,
    qt_meta_data_Razix__Editor__RZEWorldSettingsWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZERendererSettingsUIWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZERendererSettingsUIWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEWorldSettingsWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZERendererSettingsUIWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
