/****************************************************************************
** Meta object code from reading C++ file 'GraphicsView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../GraphicsView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphicsView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GraphicsView_t {
    QByteArrayData data[26];
    char stringdata0[313];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GraphicsView_t qt_meta_stringdata_GraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GraphicsView"
QT_MOC_LITERAL(1, 13, 7), // "addRect"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "Rect"
QT_MOC_LITERAL(4, 27, 21), // "CannyThresholdChanged"
QT_MOC_LITERAL(5, 49, 6), // "int8_t"
QT_MOC_LITERAL(6, 56, 4), // "LorH"
QT_MOC_LITERAL(7, 61, 6), // "size_t"
QT_MOC_LITERAL(8, 68, 19), // "CannyThresholdValue"
QT_MOC_LITERAL(9, 88, 15), // "RefreshViewBox2"
QT_MOC_LITERAL(10, 104, 6), // "Center"
QT_MOC_LITERAL(11, 111, 4), // "Zoom"
QT_MOC_LITERAL(12, 116, 11), // "scaleFactor"
QT_MOC_LITERAL(13, 128, 9), // "Translate"
QT_MOC_LITERAL(14, 138, 5), // "delta"
QT_MOC_LITERAL(15, 144, 8), // "fitFrame"
QT_MOC_LITERAL(16, 153, 28), // "lowThresholdValueChangeEvent"
QT_MOC_LITERAL(17, 182, 5), // "value"
QT_MOC_LITERAL(18, 188, 29), // "highThresholdValueChangeEvent"
QT_MOC_LITERAL(19, 218, 18), // "GenerateConvexHull"
QT_MOC_LITERAL(20, 237, 15), // "StartFineTuning"
QT_MOC_LITERAL(21, 253, 12), // "StartAddMask"
QT_MOC_LITERAL(22, 266, 15), // "StartRemoveMask"
QT_MOC_LITERAL(23, 282, 10), // "ExportMask"
QT_MOC_LITERAL(24, 293, 9), // "NextImage"
QT_MOC_LITERAL(25, 303, 9) // "LastImage"

    },
    "GraphicsView\0addRect\0\0Rect\0"
    "CannyThresholdChanged\0int8_t\0LorH\0"
    "size_t\0CannyThresholdValue\0RefreshViewBox2\0"
    "Center\0Zoom\0scaleFactor\0Translate\0"
    "delta\0fitFrame\0lowThresholdValueChangeEvent\0"
    "value\0highThresholdValueChangeEvent\0"
    "GenerateConvexHull\0StartFineTuning\0"
    "StartAddMask\0StartRemoveMask\0ExportMask\0"
    "NextImage\0LastImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   99,    2, 0x06 /* Public */,
       4,    2,  102,    2, 0x06 /* Public */,
       9,    0,  107,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,  108,    2, 0x0a /* Public */,
      11,    1,  109,    2, 0x0a /* Public */,
      11,    0,  112,    2, 0x2a /* Public | MethodCloned */,
      13,    1,  113,    2, 0x0a /* Public */,
      15,    0,  116,    2, 0x0a /* Public */,
      16,    1,  117,    2, 0x0a /* Public */,
      18,    1,  120,    2, 0x0a /* Public */,
      19,    0,  123,    2, 0x0a /* Public */,
      20,    0,  124,    2, 0x0a /* Public */,
      21,    0,  125,    2, 0x0a /* Public */,
      22,    0,  126,    2, 0x0a /* Public */,
      23,    0,  127,    2, 0x0a /* Public */,
      24,    0,  128,    2, 0x0a /* Public */,
      25,    0,  129,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QRectF,    3,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7,    6,    8,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GraphicsView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addRect((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 1: _t->CannyThresholdChanged((*reinterpret_cast< int8_t(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2]))); break;
        case 2: _t->RefreshViewBox2(); break;
        case 3: _t->Center(); break;
        case 4: _t->Zoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->Zoom(); break;
        case 6: _t->Translate((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 7: _t->fitFrame(); break;
        case 8: _t->lowThresholdValueChangeEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->highThresholdValueChangeEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->GenerateConvexHull(); break;
        case 11: _t->StartFineTuning(); break;
        case 12: _t->StartAddMask(); break;
        case 13: _t->StartRemoveMask(); break;
        case 14: _t->ExportMask(); break;
        case 15: _t->NextImage(); break;
        case 16: _t->LastImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GraphicsView::*)(QRectF );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::addRect)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GraphicsView::*)(int8_t , size_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::CannyThresholdChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::RefreshViewBox2)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_GraphicsView.data,
    qt_meta_data_GraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int GraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void GraphicsView::addRect(QRectF _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GraphicsView::CannyThresholdChanged(int8_t _t1, size_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GraphicsView::RefreshViewBox2()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
