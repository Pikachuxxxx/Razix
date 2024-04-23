/****************************************************************************
** Meta object code from reading C++ file 'RZERendererSettingsUIWindow.h'
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
#error "The header file 'RZERendererSettingsUIWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow_t {
    QByteArrayData data[15];
    char stringdata0[273];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow_t qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow = {
    {
QT_MOC_LITERAL(0, 0, 42), // "Razix::Editor::RZERendererSet..."
QT_MOC_LITERAL(1, 43, 22), // "On_UseProceduralSkybox"
QT_MOC_LITERAL(2, 66, 0), // ""
QT_MOC_LITERAL(3, 67, 20), // "OnToneMapModeChanged"
QT_MOC_LITERAL(4, 88, 15), // "OnAAModeChanged"
QT_MOC_LITERAL(5, 104, 29), // "OnSceneSamplingPatternChanged"
QT_MOC_LITERAL(6, 134, 17), // "OnSetFilterRadius"
QT_MOC_LITERAL(7, 152, 13), // "OnSetStrength"
QT_MOC_LITERAL(8, 166, 13), // "OnDebugVisCSM"
QT_MOC_LITERAL(9, 180, 16), // "OnShadowsEnabled"
QT_MOC_LITERAL(10, 197, 15), // "OnSkyboxEnabled"
QT_MOC_LITERAL(11, 213, 13), // "OnSSAOEnabled"
QT_MOC_LITERAL(12, 227, 14), // "OnImGuiEnabled"
QT_MOC_LITERAL(13, 242, 13), // "OnFXAAEnabled"
QT_MOC_LITERAL(14, 256, 16) // "OnTonemapEnabled"

    },
    "Razix::Editor::RZERendererSettingsUIWindow\0"
    "On_UseProceduralSkybox\0\0OnToneMapModeChanged\0"
    "OnAAModeChanged\0OnSceneSamplingPatternChanged\0"
    "OnSetFilterRadius\0OnSetStrength\0"
    "OnDebugVisCSM\0OnShadowsEnabled\0"
    "OnSkyboxEnabled\0OnSSAOEnabled\0"
    "OnImGuiEnabled\0OnFXAAEnabled\0"
    "OnTonemapEnabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZERendererSettingsUIWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    0,   80,    2, 0x0a /* Public */,
       4,    0,   81,    2, 0x0a /* Public */,
       5,    0,   82,    2, 0x0a /* Public */,
       6,    0,   83,    2, 0x0a /* Public */,
       7,    0,   84,    2, 0x0a /* Public */,
       8,    0,   85,    2, 0x0a /* Public */,
       9,    0,   86,    2, 0x0a /* Public */,
      10,    0,   87,    2, 0x0a /* Public */,
      11,    0,   88,    2, 0x0a /* Public */,
      12,    0,   89,    2, 0x0a /* Public */,
      13,    0,   90,    2, 0x0a /* Public */,
      14,    0,   91,    2, 0x0a /* Public */,

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
        case 6: _t->OnDebugVisCSM(); break;
        case 7: _t->OnShadowsEnabled(); break;
        case 8: _t->OnSkyboxEnabled(); break;
        case 9: _t->OnSSAOEnabled(); break;
        case 10: _t->OnImGuiEnabled(); break;
        case 11: _t->OnFXAAEnabled(); break;
        case 12: _t->OnTonemapEnabled(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZERendererSettingsUIWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow.data,
    qt_meta_data_Razix__Editor__RZERendererSettingsUIWindow,
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
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZERendererSettingsUIWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZERendererSettingsUIWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
