/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerKSliceFooBarWidget.h"
#include "ui_qSlicerKSliceFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_KSlice
class qSlicerKSliceFooBarWidgetPrivate
  : public Ui_qSlicerKSliceFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerKSliceFooBarWidget);
protected:
  qSlicerKSliceFooBarWidget* const q_ptr;

public:
  qSlicerKSliceFooBarWidgetPrivate(
    qSlicerKSliceFooBarWidget& object);
  virtual void setupUi(qSlicerKSliceFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerKSliceFooBarWidgetPrivate
::qSlicerKSliceFooBarWidgetPrivate(
  qSlicerKSliceFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerKSliceFooBarWidgetPrivate
::setupUi(qSlicerKSliceFooBarWidget* widget)
{
  this->Ui_qSlicerKSliceFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerKSliceFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerKSliceFooBarWidget
::qSlicerKSliceFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerKSliceFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerKSliceFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerKSliceFooBarWidget
::~qSlicerKSliceFooBarWidget()
{
}
