/****************************************************************************
** Meta object code from reading C++ file 'activitieslistitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ui/activitieslistitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'activitieslistitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ActivitiesListItemWidget_t {
    QByteArrayData data[8];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActivitiesListItemWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActivitiesListItemWidget_t qt_meta_stringdata_ActivitiesListItemWidget = {
    {
QT_MOC_LITERAL(0, 0, 24), // "ActivitiesListItemWidget"
QT_MOC_LITERAL(1, 25, 8), // "selected"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 12), // "wantToDelete"
QT_MOC_LITERAL(4, 48, 10), // "wantToEdit"
QT_MOC_LITERAL(5, 59, 14), // "activityEdited"
QT_MOC_LITERAL(6, 74, 8), // "Activity"
QT_MOC_LITERAL(7, 83, 8) // "activity"

    },
    "ActivitiesListItemWidget\0selected\0\0"
    "wantToDelete\0wantToEdit\0activityEdited\0"
    "Activity\0activity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActivitiesListItemWidget[] = {

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
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void ActivitiesListItemWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ActivitiesListItemWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selected(); break;
        case 1: _t->wantToDelete(); break;
        case 2: _t->wantToEdit(); break;
        case 3: _t->activityEdited((*reinterpret_cast< const Activity(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ActivitiesListItemWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ActivitiesListItemWidget::selected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ActivitiesListItemWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ActivitiesListItemWidget::wantToDelete)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ActivitiesListItemWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ActivitiesListItemWidget::wantToEdit)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ActivitiesListItemWidget::*)(const Activity & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ActivitiesListItemWidget::activityEdited)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ActivitiesListItemWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ActivitiesListItemWidget.data,
    qt_meta_data_ActivitiesListItemWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ActivitiesListItemWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActivitiesListItemWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ActivitiesListItemWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ActivitiesListItemWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ActivitiesListItemWidget::selected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ActivitiesListItemWidget::wantToDelete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ActivitiesListItemWidget::wantToEdit()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ActivitiesListItemWidget::activityEdited(const Activity & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
