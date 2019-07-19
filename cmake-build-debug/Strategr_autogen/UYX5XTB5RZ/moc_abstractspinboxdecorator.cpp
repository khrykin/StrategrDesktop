/****************************************************************************
** Meta object code from reading C++ file 'abstractspinboxdecorator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ui/abstractspinboxdecorator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractspinboxdecorator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AbstractSpinBoxDecorator_t {
    QByteArrayData data[4];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AbstractSpinBoxDecorator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AbstractSpinBoxDecorator_t qt_meta_stringdata_AbstractSpinBoxDecorator = {
    {
QT_MOC_LITERAL(0, 0, 24), // "AbstractSpinBoxDecorator"
QT_MOC_LITERAL(1, 25, 6), // "stepUp"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8) // "stepDown"

    },
    "AbstractSpinBoxDecorator\0stepUp\0\0"
    "stepDown"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AbstractSpinBoxDecorator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AbstractSpinBoxDecorator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AbstractSpinBoxDecorator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stepUp(); break;
        case 1: _t->stepDown(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject AbstractSpinBoxDecorator::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_AbstractSpinBoxDecorator.data,
    qt_meta_data_AbstractSpinBoxDecorator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AbstractSpinBoxDecorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AbstractSpinBoxDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AbstractSpinBoxDecorator.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AbstractSpinBoxDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_SpinBoxDecorator_t {
    QByteArrayData data[4];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpinBoxDecorator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpinBoxDecorator_t qt_meta_stringdata_SpinBoxDecorator = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SpinBoxDecorator"
QT_MOC_LITERAL(1, 17, 12), // "valueChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5) // "value"

    },
    "SpinBoxDecorator\0valueChanged\0\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpinBoxDecorator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void SpinBoxDecorator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpinBoxDecorator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpinBoxDecorator::staticMetaObject = { {
    &AbstractSpinBoxDecorator::staticMetaObject,
    qt_meta_stringdata_SpinBoxDecorator.data,
    qt_meta_data_SpinBoxDecorator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpinBoxDecorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpinBoxDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpinBoxDecorator.stringdata0))
        return static_cast<void*>(this);
    return AbstractSpinBoxDecorator::qt_metacast(_clname);
}

int SpinBoxDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractSpinBoxDecorator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_TimeEditDecorator_t {
    QByteArrayData data[4];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TimeEditDecorator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TimeEditDecorator_t qt_meta_stringdata_TimeEditDecorator = {
    {
QT_MOC_LITERAL(0, 0, 17), // "TimeEditDecorator"
QT_MOC_LITERAL(1, 18, 11), // "timeChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 4) // "time"

    },
    "TimeEditDecorator\0timeChanged\0\0time"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TimeEditDecorator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QTime,    3,

       0        // eod
};

void TimeEditDecorator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TimeEditDecorator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->timeChanged((*reinterpret_cast< QTime(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TimeEditDecorator::staticMetaObject = { {
    &AbstractSpinBoxDecorator::staticMetaObject,
    qt_meta_stringdata_TimeEditDecorator.data,
    qt_meta_data_TimeEditDecorator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TimeEditDecorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimeEditDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TimeEditDecorator.stringdata0))
        return static_cast<void*>(this);
    return AbstractSpinBoxDecorator::qt_metacast(_clname);
}

int TimeEditDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractSpinBoxDecorator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
