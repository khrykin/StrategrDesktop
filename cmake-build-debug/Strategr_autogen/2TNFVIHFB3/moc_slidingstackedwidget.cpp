/****************************************************************************
** Meta object code from reading C++ file 'slidingstackedwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../third-party/slidingstackedwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'slidingstackedwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SlidingStackedWidget_t {
    QByteArrayData data[22];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SlidingStackedWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SlidingStackedWidget_t qt_meta_stringdata_SlidingStackedWidget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SlidingStackedWidget"
QT_MOC_LITERAL(1, 21, 17), // "animationFinished"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 8), // "setSpeed"
QT_MOC_LITERAL(4, 49, 5), // "speed"
QT_MOC_LITERAL(5, 55, 12), // "setAnimation"
QT_MOC_LITERAL(6, 68, 18), // "QEasingCurve::Type"
QT_MOC_LITERAL(7, 87, 13), // "animationtype"
QT_MOC_LITERAL(8, 101, 15), // "setVerticalMode"
QT_MOC_LITERAL(9, 117, 8), // "vertical"
QT_MOC_LITERAL(10, 126, 7), // "setWrap"
QT_MOC_LITERAL(11, 134, 4), // "wrap"
QT_MOC_LITERAL(12, 139, 11), // "slideInNext"
QT_MOC_LITERAL(13, 151, 11), // "slideInPrev"
QT_MOC_LITERAL(14, 163, 12), // "slideToIndex"
QT_MOC_LITERAL(15, 176, 3), // "idx"
QT_MOC_LITERAL(16, 180, 11), // "t_direction"
QT_MOC_LITERAL(17, 192, 9), // "direction"
QT_MOC_LITERAL(18, 202, 13), // "slideToWidget"
QT_MOC_LITERAL(19, 216, 8), // "QWidget*"
QT_MOC_LITERAL(20, 225, 6), // "widget"
QT_MOC_LITERAL(21, 232, 17) // "animationDoneSlot"

    },
    "SlidingStackedWidget\0animationFinished\0"
    "\0setSpeed\0speed\0setAnimation\0"
    "QEasingCurve::Type\0animationtype\0"
    "setVerticalMode\0vertical\0setWrap\0wrap\0"
    "slideInNext\0slideInPrev\0slideToIndex\0"
    "idx\0t_direction\0direction\0slideToWidget\0"
    "QWidget*\0widget\0animationDoneSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SlidingStackedWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   80,    2, 0x0a /* Public */,
       5,    1,   83,    2, 0x0a /* Public */,
       8,    1,   86,    2, 0x0a /* Public */,
       8,    0,   89,    2, 0x2a /* Public | MethodCloned */,
      10,    1,   90,    2, 0x0a /* Public */,
      12,    0,   93,    2, 0x0a /* Public */,
      13,    0,   94,    2, 0x0a /* Public */,
      14,    2,   95,    2, 0x0a /* Public */,
      14,    1,  100,    2, 0x2a /* Public | MethodCloned */,
      18,    2,  103,    2, 0x0a /* Public */,
      18,    1,  108,    2, 0x2a /* Public | MethodCloned */,
      21,    0,  111,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 16,   15,   17,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, 0x80000000 | 19, 0x80000000 | 16,   20,   17,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void,

       0        // eod
};

void SlidingStackedWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SlidingStackedWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->animationFinished(); break;
        case 1: _t->setSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setAnimation((*reinterpret_cast< QEasingCurve::Type(*)>(_a[1]))); break;
        case 3: _t->setVerticalMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setVerticalMode(); break;
        case 5: _t->setWrap((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->slideInNext(); break;
        case 7: _t->slideInPrev(); break;
        case 8: _t->slideToIndex((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< t_direction(*)>(_a[2]))); break;
        case 9: _t->slideToIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->slideToWidget((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< t_direction(*)>(_a[2]))); break;
        case 11: _t->slideToWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 12: _t->animationDoneSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SlidingStackedWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SlidingStackedWidget::animationFinished)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SlidingStackedWidget::staticMetaObject = { {
    &QStackedWidget::staticMetaObject,
    qt_meta_stringdata_SlidingStackedWidget.data,
    qt_meta_data_SlidingStackedWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SlidingStackedWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SlidingStackedWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SlidingStackedWidget.stringdata0))
        return static_cast<void*>(this);
    return QStackedWidget::qt_metacast(_clname);
}

int SlidingStackedWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void SlidingStackedWidget::animationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
