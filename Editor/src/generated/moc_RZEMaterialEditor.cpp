/****************************************************************************
** Meta object code from reading C++ file 'RZEMaterialEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rzepch.h"
#include "../UI/Windows/RZEMaterialEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RZEMaterialEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Razix__Editor__RZEMaterialEditor_t {
    QByteArrayData data[15];
    char stringdata0[326];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Razix__Editor__RZEMaterialEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Razix__Editor__RZEMaterialEditor_t qt_meta_stringdata_Razix__Editor__RZEMaterialEditor = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Razix::Editor::RZEMaterialEditor"
QT_MOC_LITERAL(1, 33, 20), // "OnSetEditingMaterial"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 28), // "Razix::Graphics::RZMaterial*"
QT_MOC_LITERAL(4, 84, 8), // "material"
QT_MOC_LITERAL(5, 93, 23), // "on_DiffuseTextureSelect"
QT_MOC_LITERAL(6, 117, 27), // "onDiffuseTextureUseCheckbox"
QT_MOC_LITERAL(7, 145, 15), // "on_DiffuseColor"
QT_MOC_LITERAL(8, 161, 26), // "on_SpecularTextureSelected"
QT_MOC_LITERAL(9, 188, 20), // "on_SpecularIntensity"
QT_MOC_LITERAL(10, 209, 19), // "on_MetallicValueSet"
QT_MOC_LITERAL(11, 229, 26), // "on_MetallicTextureSelected"
QT_MOC_LITERAL(12, 256, 20), // "on_RoughnessValueSet"
QT_MOC_LITERAL(13, 277, 27), // "on_RoughnessTextureSelected"
QT_MOC_LITERAL(14, 305, 20) // "on_EmissionIntensity"

    },
    "Razix::Editor::RZEMaterialEditor\0"
    "OnSetEditingMaterial\0\0"
    "Razix::Graphics::RZMaterial*\0material\0"
    "on_DiffuseTextureSelect\0"
    "onDiffuseTextureUseCheckbox\0on_DiffuseColor\0"
    "on_SpecularTextureSelected\0"
    "on_SpecularIntensity\0on_MetallicValueSet\0"
    "on_MetallicTextureSelected\0"
    "on_RoughnessValueSet\0on_RoughnessTextureSelected\0"
    "on_EmissionIntensity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Razix__Editor__RZEMaterialEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x0a /* Public */,
       5,    0,   72,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x0a /* Public */,
       7,    0,   74,    2, 0x0a /* Public */,
       8,    0,   75,    2, 0x0a /* Public */,
       9,    0,   76,    2, 0x0a /* Public */,
      10,    0,   77,    2, 0x0a /* Public */,
      11,    0,   78,    2, 0x0a /* Public */,
      12,    0,   79,    2, 0x0a /* Public */,
      13,    0,   80,    2, 0x0a /* Public */,
      14,    0,   81,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
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

void Razix::Editor::RZEMaterialEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RZEMaterialEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnSetEditingMaterial((*reinterpret_cast< Razix::Graphics::RZMaterial*(*)>(_a[1]))); break;
        case 1: _t->on_DiffuseTextureSelect(); break;
        case 2: _t->onDiffuseTextureUseCheckbox(); break;
        case 3: _t->on_DiffuseColor(); break;
        case 4: _t->on_SpecularTextureSelected(); break;
        case 5: _t->on_SpecularIntensity(); break;
        case 6: _t->on_MetallicValueSet(); break;
        case 7: _t->on_MetallicTextureSelected(); break;
        case 8: _t->on_RoughnessValueSet(); break;
        case 9: _t->on_RoughnessTextureSelected(); break;
        case 10: _t->on_EmissionIntensity(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Razix::Editor::RZEMaterialEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Razix__Editor__RZEMaterialEditor.data,
    qt_meta_data_Razix__Editor__RZEMaterialEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Razix::Editor::RZEMaterialEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Razix::Editor::RZEMaterialEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Razix__Editor__RZEMaterialEditor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Razix::Editor::RZEMaterialEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
