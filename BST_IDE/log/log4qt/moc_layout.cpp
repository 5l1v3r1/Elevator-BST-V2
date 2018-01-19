/****************************************************************************
** Meta object code from reading C++ file 'layout.h'
**
** Created: Tue Nov 26 10:14:54 2013
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "layout.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Log4Qt__Layout[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      23,   15, 0x0a095001,
      41,   15, 0x0a095103,
      48,   15, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_Log4Qt__Layout[] = {
    "Log4Qt::Layout\0QString\0footercontentType\0"
    "footer\0header\0"
};

const QMetaObject Log4Qt::Layout::staticMetaObject = {
    { &LogObject::staticMetaObject, qt_meta_stringdata_Log4Qt__Layout,
      qt_meta_data_Log4Qt__Layout, 0 }
};

const QMetaObject *Log4Qt::Layout::metaObject() const
{
    return &staticMetaObject;
}

void *Log4Qt::Layout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Log4Qt__Layout))
        return static_cast<void*>(const_cast< Layout*>(this));
    return LogObject::qt_metacast(_clname);
}

int Log4Qt::Layout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LogObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = contentType(); break;
        case 1: *reinterpret_cast< QString*>(_v) = footer(); break;
        case 2: *reinterpret_cast< QString*>(_v) = header(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setFooter(*reinterpret_cast< QString*>(_v)); break;
        case 2: setHeader(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
