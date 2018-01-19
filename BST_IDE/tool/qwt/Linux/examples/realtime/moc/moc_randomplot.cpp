/****************************************************************************
** Meta object code from reading C++ file 'randomplot.h'
**
** Created: Sat Oct 12 10:00:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../randomplot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'randomplot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RandomPlot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,   11,   11,   11, 0x0a,
      34,   11,   11,   11, 0x0a,
      55,   41,   11,   11, 0x0a,
      71,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RandomPlot[] = {
    "RandomPlot\0\0running(bool)\0clear()\0"
    "stop()\0timeout,count\0append(int,int)\0"
    "appendPoint()\0"
};

void RandomPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RandomPlot *_t = static_cast<RandomPlot *>(_o);
        switch (_id) {
        case 0: _t->running((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->clear(); break;
        case 2: _t->stop(); break;
        case 3: _t->append((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->appendPoint(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RandomPlot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RandomPlot::staticMetaObject = {
    { &IncrementalPlot::staticMetaObject, qt_meta_stringdata_RandomPlot,
      qt_meta_data_RandomPlot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RandomPlot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RandomPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RandomPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RandomPlot))
        return static_cast<void*>(const_cast< RandomPlot*>(this));
    return IncrementalPlot::qt_metacast(_clname);
}

int RandomPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IncrementalPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RandomPlot::running(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
