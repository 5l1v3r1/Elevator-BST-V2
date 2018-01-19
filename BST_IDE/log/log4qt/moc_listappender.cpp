/****************************************************************************
** Meta object code from reading C++ file 'listappender.h'
**
** Created: Tue Nov 26 10:15:01 2013
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "varia/listappender.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'listappender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Log4Qt__ListAppender[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      26,   21, 0x01095103,
      47,   43, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_Log4Qt__ListAppender[] = {
    "Log4Qt::ListAppender\0bool\0configuratorList\0"
    "int\0maxCount\0"
};

const QMetaObject Log4Qt::ListAppender::staticMetaObject = {
    { &AppenderSkeleton::staticMetaObject, qt_meta_stringdata_Log4Qt__ListAppender,
      qt_meta_data_Log4Qt__ListAppender, 0 }
};

const QMetaObject *Log4Qt::ListAppender::metaObject() const
{
    return &staticMetaObject;
}

void *Log4Qt::ListAppender::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Log4Qt__ListAppender))
        return static_cast<void*>(const_cast< ListAppender*>(this));
    return AppenderSkeleton::qt_metacast(_clname);
}

int Log4Qt::ListAppender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AppenderSkeleton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = configuratorList(); break;
        case 1: *reinterpret_cast< int*>(_v) = maxCount(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setConfiguratorList(*reinterpret_cast< bool*>(_v)); break;
        case 1: setMaxCount(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
