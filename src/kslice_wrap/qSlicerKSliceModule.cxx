/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// KSlice Logic includes
#include <vtkSlicerKSliceLogic.h>

// KSlice includes
#include "qSlicerKSliceModule.h"
#include "qSlicerKSliceModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerKSliceModule, qSlicerKSliceModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerKSliceModulePrivate
{
public:
  qSlicerKSliceModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerKSliceModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerKSliceModulePrivate
::qSlicerKSliceModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerKSliceModule methods

//-----------------------------------------------------------------------------
qSlicerKSliceModule
::qSlicerKSliceModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerKSliceModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerKSliceModule::~qSlicerKSliceModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerKSliceModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerKSliceModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerKSliceModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Ivan Kolesov, Peter Karasev (Georgia Tech)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerKSliceModule::icon()const
{
  return QIcon(":/Icons/KSlice.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerKSliceModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerKSliceModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerKSliceModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerKSliceModule
::createWidgetRepresentation()
{
  return new qSlicerKSliceModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerKSliceModule::createLogic()
{
  return vtkSlicerKSliceLogic::New();
}
