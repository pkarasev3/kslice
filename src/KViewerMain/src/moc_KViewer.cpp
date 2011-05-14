/****************************************************************************
** Meta object code from reading C++ file 'KViewer.h'
**
** Created: Fri May 13 20:21:04 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/KViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KViewer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      44,   34,    8,    8, 0x0a,
      61,    8,    8,    8, 0x0a,
      83,    8,    8,    8, 0x0a,
      99,    8,    8,    8, 0x0a,
     118,    8,    8,    8, 0x0a,
     143,  139,    8,    8, 0x0a,
     171,    8,    8,    8, 0x0a,
     183,    8,    8,    8, 0x0a,
     198,    8,    8,    8, 0x0a,
     206,    8,    8,    8, 0x0a,
     241,    8,    8,    8, 0x0a,
     262,    8,    8,    8, 0x0a,
     283,    8,  279,    8, 0x0a,
     314,  306,    8,    8, 0x0a,
     345,    8,    8,    8, 0x2a,
     373,    8,    8,    8, 0x0a,
     416,  406,    8,    8, 0x0a,
     453,    8,    8,    8, 0x0a,
     476,    8,    8,    8, 0x0a,
     518,  498,    8,    8, 0x0a,
     534,    8,    8,    8, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_KViewer[] = {
    "KViewer\0\0updateCoords(vtkObject*)\0"
    "SliderVal\0SliceSelect(int)\0"
    "ToggleFillEraseMode()\0UpdateModel3D()\0"
    "SaveSegmentation()\0SaveAsSegmentation()\0"
    "obj\0mousePaintEvent(vtkObject*)\0"
    "LoadImage()\0LoadLabelMap()\0About()\0"
    "updatePaintBrushStatus(vtkObject*)\0"
    "UpdateVolumeStatus()\0AddNewLabelMap()\0"
    "int\0getCurrentSliceIndex()\0idxFrom\0"
    "queueCopyFromCurrentSlice(int)\0"
    "queueCopyFromCurrentSlice()\0"
    "executeSingleSliceSegmentation()\0"
    "obj,event\0handleGenericEvent(vtkObject*,ulong)\0"
    "copyFromCurrentSlice()\0pasteToCurrentSlice()\0"
    "shiftNumberOfSlices\0MoveSlider(int)\0"
    "MoveSlider()\0"
};

const QMetaObject KViewer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_KViewer,
      qt_meta_data_KViewer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KViewer))
        return static_cast<void*>(const_cast< KViewer*>(this));
    if (!strcmp(_clname, "Ui::GUI"))
        return static_cast< Ui::GUI*>(const_cast< KViewer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int KViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateCoords((*reinterpret_cast< vtkObject*(*)>(_a[1]))); break;
        case 1: SliceSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: ToggleFillEraseMode(); break;
        case 3: UpdateModel3D(); break;
        case 4: SaveSegmentation(); break;
        case 5: SaveAsSegmentation(); break;
        case 6: mousePaintEvent((*reinterpret_cast< vtkObject*(*)>(_a[1]))); break;
        case 7: LoadImage(); break;
        case 8: LoadLabelMap(); break;
        case 9: About(); break;
        case 10: updatePaintBrushStatus((*reinterpret_cast< vtkObject*(*)>(_a[1]))); break;
        case 11: UpdateVolumeStatus(); break;
        case 12: AddNewLabelMap(); break;
        case 13: { int _r = getCurrentSliceIndex();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 14: queueCopyFromCurrentSlice((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: queueCopyFromCurrentSlice(); break;
        case 16: executeSingleSliceSegmentation(); break;
        case 17: handleGenericEvent((*reinterpret_cast< vtkObject*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2]))); break;
        case 18: copyFromCurrentSlice(); break;
        case 19: pasteToCurrentSlice(); break;
        case 20: MoveSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: MoveSlider(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
