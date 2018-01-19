/****************************************************************************
** Meta object code from reading C++ file 'denyallfilter.h'
**
** Created: Tue Nov 26 10:14:59 2013
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "varia/denyallfilter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'denyallfilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Log4Qt__DenyAllFilter[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_Log4Qt__DenyAllFilter[] = {
    "Log4Qt::DenyAllFilter\0"
};

const QMetaObject Log4Qt::DenyAllFilter::staticMetaObject = {
    { &Filter::staticMetaObject, qt_meta_stringdata_Log4Qt__DenyAllFilter,
      qt_meta_data_Log4Qt__DenyAllFilter, 0 }
};

const QMetaObject *Log4Qt::DenyAllFilter::metaObject() const
{
    return &staticMetaObject;
}

void *Log4Qt::DenyAllFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Log4Qt__DenyAllFilter))
        return static_cast<void*>(const_cast< DenyAllFilter*>(this));
    return Filter::qt_metacast(_clname);
}

int Log4Qt::DenyAllFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Filter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
