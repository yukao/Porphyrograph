/****************************************************************************
** Meta object code from reading C++ file 'pg-interface-song-slidersgroup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../pg-interface-song-slidersgroup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pg-interface-song-slidersgroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DoubleSlider_t {
    QByteArrayData data[8];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DoubleSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DoubleSlider_t qt_meta_stringdata_DoubleSlider = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DoubleSlider"
QT_MOC_LITERAL(1, 13, 18), // "doubleValueChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "newValue"
QT_MOC_LITERAL(4, 42, 15), // "intValueChanged"
QT_MOC_LITERAL(5, 58, 25), // "setInversedScaledIntValue"
QT_MOC_LITERAL(6, 84, 10), // "givenValue"
QT_MOC_LITERAL(7, 95, 20) // "setScaledDoubleValue"

    },
    "DoubleSlider\0doubleValueChanged\0\0"
    "newValue\0intValueChanged\0"
    "setInversedScaledIntValue\0givenValue\0"
    "setScaledDoubleValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DoubleSlider[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   40,    2, 0x0a /* Public */,
       7,    1,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Double,    6,

       0        // eod
};

void DoubleSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DoubleSlider *_t = static_cast<DoubleSlider *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doubleValueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->intValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setInversedScaledIntValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setScaledDoubleValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DoubleSlider::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DoubleSlider::doubleValueChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DoubleSlider::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DoubleSlider::intValueChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject DoubleSlider::staticMetaObject = {
    { &QSlider::staticMetaObject, qt_meta_stringdata_DoubleSlider.data,
      qt_meta_data_DoubleSlider,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DoubleSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DoubleSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DoubleSlider.stringdata0))
        return static_cast<void*>(const_cast< DoubleSlider*>(this));
    return QSlider::qt_metacast(_clname);
}

int DoubleSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSlider::qt_metacall(_c, _id, _a);
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
void DoubleSlider::doubleValueChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DoubleSlider::intValueChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_SlidersSpinboxGroup_t {
    QByteArrayData data[6];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SlidersSpinboxGroup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SlidersSpinboxGroup_t qt_meta_stringdata_SlidersSpinboxGroup = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SlidersSpinboxGroup"
QT_MOC_LITERAL(1, 20, 19), // "setIntValueFromMIDI"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 10), // "givenValue"
QT_MOC_LITERAL(4, 52, 21), // "setDoubleValueFromUDP"
QT_MOC_LITERAL(5, 74, 14) // "sendUDPMessage"

    },
    "SlidersSpinboxGroup\0setIntValueFromMIDI\0"
    "\0givenValue\0setDoubleValueFromUDP\0"
    "sendUDPMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SlidersSpinboxGroup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    1,   32,    2, 0x0a /* Public */,
       5,    1,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void SlidersSpinboxGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SlidersSpinboxGroup *_t = static_cast<SlidersSpinboxGroup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setIntValueFromMIDI((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setDoubleValueFromUDP((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->sendUDPMessage((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SlidersSpinboxGroup::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_SlidersSpinboxGroup.data,
      qt_meta_data_SlidersSpinboxGroup,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SlidersSpinboxGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SlidersSpinboxGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SlidersSpinboxGroup.stringdata0))
        return static_cast<void*>(const_cast< SlidersSpinboxGroup*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int SlidersSpinboxGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_SingleButton_t {
    QByteArrayData data[4];
    char stringdata0[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SingleButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SingleButton_t qt_meta_stringdata_SingleButton = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SingleButton"
QT_MOC_LITERAL(1, 13, 14), // "sendUDPMessage"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 11) // "handleEvent"

    },
    "SingleButton\0sendUDPMessage\0\0handleEvent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SingleButton[] = {

 // content:
       7,       // revision
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

void SingleButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SingleButton *_t = static_cast<SingleButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendUDPMessage(); break;
        case 1: _t->handleEvent(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SingleButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_SingleButton.data,
      qt_meta_data_SingleButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SingleButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SingleButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SingleButton.stringdata0))
        return static_cast<void*>(const_cast< SingleButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int SingleButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
