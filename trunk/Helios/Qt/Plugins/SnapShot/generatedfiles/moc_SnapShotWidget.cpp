/****************************************************************************
** Meta object code from reading C++ file 'SnapShotWidget.h'
**
** Created: Sat Nov 24 16:55:18 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SnapShotWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SnapShotWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_SnapShotWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SnapShotWidget[] = {
    "SnapShotWidget\0\0on__snapShotButton_clicked()\0"
};

const QMetaObject SnapShotWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SnapShotWidget,
      qt_meta_data_SnapShotWidget, 0 }
};

const QMetaObject *SnapShotWidget::metaObject() const
{
    return &staticMetaObject;
}

void *SnapShotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SnapShotWidget))
	return static_cast<void*>(const_cast< SnapShotWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SnapShotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on__snapShotButton_clicked(); break;
        }
        _id -= 1;
    }
    return _id;
}
