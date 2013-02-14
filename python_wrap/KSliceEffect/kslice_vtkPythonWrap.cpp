// python wrapper for vtkKSlice object
//
#define VTK_WRAPPING_CXX
#define VTK_STREAMS_FWD_ONLY
#include "vtkPython.h"
#undef _XOPEN_SOURCE /* Conflicts with standards.h.  */
#undef _THREAD_SAFE /* Conflicts with pthread.h.  */
#include "vtkPythonUtil.h"
#include <vtksys/ios/sstream>
#include "vtkKSlice.h"
#if defined(WIN32)
extern "C" { __declspec( dllexport ) PyObject *PyVTKClass_vtkKSliceNew(char *); }
#else
extern "C" { PyObject *PyVTKClass_vtkKSliceNew(char *); }
#endif

extern "C" { PyObject *PyVTKClass_vtkObjectNew(char *); }

static PyObject *PyvtkKSlice_SetImageVol(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  vtkImageData  *temp0;
  PyObject *tempH0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"O", &tempH0);
  if (op)
    {
    temp0 = (vtkImageData *)vtkPythonGetPointerFromObject(tempH0,(char*)"vtkImageData");
    if (!temp0 && tempH0 != Py_None) goto break0;
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetImageVol(temp0);
      }
    else
      {
      op->SetImageVol(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
 break0:
  return NULL;
}


static PyObject *PyvtkKSlice_SetLabelVol(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  vtkImageData  *temp0;
  PyObject *tempH0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"O", &tempH0);
  if (op)
    {
    temp0 = (vtkImageData *)vtkPythonGetPointerFromObject(tempH0,(char*)"vtkImageData");
    if (!temp0 && tempH0 != Py_None) goto break1;
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetLabelVol(temp0);
      }
    else
      {
      op->SetLabelVol(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
 break1:
  return NULL;
}


static PyObject *PyvtkKSlice_SetUIVol(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  vtkImageData  *temp0;
  PyObject *tempH0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"O", &tempH0);
  if (op)
    {
    temp0 = (vtkImageData *)vtkPythonGetPointerFromObject(tempH0,(char*)"vtkImageData");
    if (!temp0 && tempH0 != Py_None) goto break2;
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetUIVol(temp0);
      }
    else
      {
      op->SetUIVol(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
 break2:
  return NULL;
}


static PyObject *PyvtkKSlice_SetNumIts(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  int      temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"i", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetNumIts(temp0);
      }
    else
      {
      op->SetNumIts(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_SetBrushRad(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  int      temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"i", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetBrushRad(temp0);
      }
    else
      {
      op->SetBrushRad(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_SetCurrSlice(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  int      temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"i", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetCurrSlice(temp0);
      }
    else
      {
      op->SetCurrSlice(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_SetDistWeight(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  float    temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"f", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetDistWeight(temp0);
      }
    else
      {
      op->SetDistWeight(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_SetFromSlice(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  int      temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"i", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::SetFromSlice(temp0);
      }
    else
      {
      op->SetFromSlice(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_PasteSlice(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  int      temp0;
  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"i", &temp0);
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::PasteSlice(temp0);
      }
    else
      {
      op->PasteSlice(temp0);
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_Initialize(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"");
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::Initialize();
      }
    else
      {
      op->Initialize();
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}


static PyObject *PyvtkKSlice_runUpdate(PyObject *self, PyObject *args)
{
  /* handle an occurrence */
  {
  vtkKSlice *op;

  op = (vtkKSlice *)PyArg_VTKParseTuple(self, args, (char*)"");
  if (op)
    {
    if (PyVTKClass_Check(self))
      {
      op->vtkKSlice::runUpdate();
      }
    else
      {
      op->runUpdate();
      }
    Py_INCREF(Py_None);
    return Py_None;
    }
  }
  return NULL;
}

static PyMethodDef PyvtkKSliceMethods[] = {
  {(char*)"SetImageVol",                (PyCFunction)PyvtkKSlice_SetImageVol, 1,
   (char*)"V.SetImageVol(vtkImageData)\nC++: void SetImageVol (vtkImageData *);\n\n"},
  {(char*)"SetLabelVol",                (PyCFunction)PyvtkKSlice_SetLabelVol, 1,
   (char*)"V.SetLabelVol(vtkImageData)\nC++: void SetLabelVol (vtkImageData *);\n\n"},
  {(char*)"SetUIVol",                (PyCFunction)PyvtkKSlice_SetUIVol, 1,
   (char*)"V.SetUIVol(vtkImageData)\nC++: void SetUIVol (vtkImageData *);\n\n"},
  {(char*)"SetNumIts",                (PyCFunction)PyvtkKSlice_SetNumIts, 1,
   (char*)"V.SetNumIts(int)\nC++: void SetNumIts (int );\n\n"},
  {(char*)"SetBrushRad",                (PyCFunction)PyvtkKSlice_SetBrushRad, 1,
   (char*)"V.SetBrushRad(int)\nC++: void SetBrushRad (int );\n\n"},
  {(char*)"SetCurrSlice",                (PyCFunction)PyvtkKSlice_SetCurrSlice, 1,
   (char*)"V.SetCurrSlice(int)\nC++: void SetCurrSlice (int );\n\n"},
  {(char*)"SetDistWeight",                (PyCFunction)PyvtkKSlice_SetDistWeight, 1,
   (char*)"V.SetDistWeight(float)\nC++: void SetDistWeight (float );\n\n"},
  {(char*)"SetFromSlice",                (PyCFunction)PyvtkKSlice_SetFromSlice, 1,
   (char*)"V.SetFromSlice(int)\nC++: void SetFromSlice (int );\n\n"},
  {(char*)"PasteSlice",                (PyCFunction)PyvtkKSlice_PasteSlice, 1,
   (char*)"V.PasteSlice(int)\nC++: void PasteSlice (int toSlice);\n\n"},
  {(char*)"Initialize",                (PyCFunction)PyvtkKSlice_Initialize, 1,
   (char*)"V.Initialize()\nC++: void Initialize ();\n\n"},
  {(char*)"runUpdate",                (PyCFunction)PyvtkKSlice_runUpdate, 1,
   (char*)"V.runUpdate()\nC++: void runUpdate ();\n\n"},
  {NULL,                       NULL, 0, NULL}
};

static const char *vtkKSliceDoc[] = {
  (char*)"vtkKSlice - no description provided.\n\n",
  "Super Class:\n\n vtkObject\n\n",
  (char*)"None provided.\n\n",
  NULL
};

static vtkObjectBase *vtkKSliceStaticNew()
{
  return vtkKSlice::New();
}

PyObject *PyVTKClass_vtkKSliceNew(char *modulename)
{
  return PyVTKClass_New(&vtkKSliceStaticNew,
                        PyvtkKSliceMethods,
                        (char*)"vtkKSlice",modulename,
                        (char**)vtkKSliceDoc,
                        PyVTKClass_vtkObjectNew(modulename));
}

