#include "KvtkImageInteractionCallback.h"
#include "ParameterWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "vtkLookupTable.h"
#include "KViewer.h"
#include "KViewerOptions.h"

using cv::Ptr;
using namespace vrcl;
using std::cout;
using std::endl;

namespace {

const char keyMinusBrushSize    ='y';
const char keyPlusBrushSize     ='x';

const char keyCopyLabelSlice    ='c';
const char keyPasteLabelSlice   ='v';

const char keyUpShiftSatRange   ='l'; // shift both min and max up
const char keyDownShiftSatRange ='k'; // shift both min and max down
const char keyUpMaxSatRange     ='j'; // shift *only max* up
const char keyDownMaxSatRange   ='J'; // shift *only max* down
const char keyDownMinSatRange   ='h'; // shift *only min* down
const char keyUpMinSatRange     ='H'; // shift *only min* up

const char keyUpLabelOpacity    ='p';
const char keyDownLabelOpacity  ='o';


}

KvtkImageInteractionCallback* KvtkImageInteractionCallback::New()
{
    return new KvtkImageInteractionCallback;
}

KvtkImageInteractionCallback::~KvtkImageInteractionCallback( )
{
    m_paramWidget.reset();
}


KvtkImageInteractionCallback::KvtkImageInteractionCallback()
{
    this->Window = NULL;
    this->m_paramWidget.reset(new KViewerParameterWidget); // = std::make_unique<KViewerParameterWidget>();

    auto cb = std::bind(&KvtkImageInteractionCallback::notifyAllFromOptions,
                         this,std::placeholders::_1);
    m_paramWidget->setOptionsUpdateCallback(cb);
}

void KvtkImageInteractionCallback::SetOptions(std::shared_ptr<KViewerOptions> kv_opts)  {
    this->kv_opts = kv_opts;
    this->m_paramWidget->populateFromOptions( this->kv_opts );
}

void KvtkImageInteractionCallback::notifyChangeBrushSize(size_t k)
{
    masterWindow->SetCircleCursorSize(k);
}

void KvtkImageInteractionCallback::SetSaturationLookupTable(vtkLookupTable* lut)
{
    this->satLUT_shared = lut;
    this->m_paramWidget->setSaturationLUT(satLUT_shared);
}

void KvtkImageInteractionCallback::notifyAllFromOptions( std::shared_ptr<KViewerOptions> opts )
{
    kv_opts = opts;
    notifyChangeBrushSize(kv_opts->paintBrushRad);
    masterWindow->UpdateMultiLabelDisplay();

    masterWindow->update();

    std::cout << " did notify in: " << __FUNCTION__ << "\n";
}

void KvtkImageInteractionCallback::Execute(vtkObject *, unsigned long event, void *)
{

  if(event == vtkCommand::LeftButtonPressEvent )
  {
    //cout<<"Left button has been pressed"<<endl;
    buttonDown = !buttonDown; // paint brush down: start draw/erase
    if(!buttonDown)
    {
        masterWindow->SetCircleCursorOpacity(0);
    }
    else
        masterWindow->SetCircleCursorOpacity(1);
  }
  else if( event == vtkCommand::KeyPressEvent )
  {
    bool   bUpdatedSatLut = false;
    double satRange[2];
    satLUT_shared->GetTableRange(satRange);
    double  stepSize                               = (satRange[1]-satRange[0])*0.01;
    vtkRenderWindowInteractor* imgWindowInteractor = this->Window->GetInteractor();
    unsigned int keyPressed = *imgWindowInteractor->GetKeySym();
    Lout(Logger::INFO) << "(uint) keyPressed: " << keyPressed << endl;
    switch ( keyPressed ) {
    case keyMinusBrushSize:
      kv_opts->paintBrushRad = kv_opts->paintBrushRad-1;
      this->notifyChangeBrushSize(kv_opts->paintBrushRad);
      break;
    case keyPlusBrushSize:
      kv_opts->paintBrushRad = kv_opts->paintBrushRad+1;
      this->notifyChangeBrushSize(kv_opts->paintBrushRad);
      break;
    case keyUpLabelOpacity:
      kv_opts->labelOpacity2D *= 1.05;
      masterWindow->UpdateMultiLabelDisplay();
      break;
    case keyDownLabelOpacity:
      kv_opts->labelOpacity2D *= 0.96;
      masterWindow->UpdateMultiLabelDisplay();
      break;
    case keyUpShiftSatRange:
      satLUT_shared->SetTableRange(satRange[0]+stepSize, satRange[1]+stepSize);
      satLUT_shared->Build();
      this->masterWindow->qVTK1->update();
      bUpdatedSatLut = true;
      break;
    case keyDownShiftSatRange:
      satLUT_shared->SetTableRange(satRange[0]-stepSize, satRange[1]-stepSize);
      satLUT_shared->Build();
      this->masterWindow->qVTK1->update();
      bUpdatedSatLut = true;
      break;
    case keyUpMaxSatRange:
      satLUT_shared->SetTableRange(satRange[0], satRange[1]+stepSize);
      satLUT_shared->Build();
      this->masterWindow->qVTK1->update();
      bUpdatedSatLut = true;
      break;
    case keyDownMaxSatRange:
      if( (satRange[1]-stepSize) > satRange[0] ) {
        satLUT_shared->SetTableRange(satRange[0], satRange[1]-stepSize);
        satLUT_shared->Build();
        this->masterWindow->qVTK1->update();
        bUpdatedSatLut = true;
      }
      break;
    case keyDownMinSatRange:
      satLUT_shared->SetTableRange(satRange[0]-stepSize, satRange[1]);
      satLUT_shared->Build();
      this->masterWindow->qVTK1->update();
      bUpdatedSatLut = true;
      break;
    case keyUpMinSatRange:
      if( (satRange[0]+stepSize) < satRange[1] ) {
        satLUT_shared->SetTableRange(satRange[0]+stepSize, satRange[1]);
        satLUT_shared->Build();
        this->masterWindow->qVTK1->update();
        bUpdatedSatLut = true;
      }
      break;
    case keyCopyLabelSlice:
      indexSliceCopyFrom = masterWindow->getCurrentSliceIndex( );
      masterWindow->queueCopyFromCurrentSlice( indexSliceCopyFrom );
      break;
    case 'w':
    case 'R':
      this->masterWindow->MoveSlider(  1 ); // right slider

      break;
    case 'q':
    case 'L':
      this->masterWindow->MoveSlider( -1 ); // left slider

      break;
    case 'd': // toggle draw/erase
      erase = ! erase;
      break;
    case '3': // base class of interactor wants to grab this key stroke...
      // need to override or undo it
      // http://www.vtk.org/doc/release/5.0/html/a01650.html
      break;
    default:
      break;
    }
    if( bUpdatedSatLut ) {
      cout << "updated satLUT range: " << satLUT_shared->GetTableRange()[0]
           << ", " << satLUT_shared->GetTableRange()[1] << endl;
    }
  }

  // trigger the main window to update text displaying paintbrush info
  this->masterWindow->updatePaintBrushStatus( NULL );

  m_paramWidget->populateFromOptions(kv_opts);


}
