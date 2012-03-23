#include "KvtkImageInteractionCallback.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "vtkLookupTable.h"
#include "KViewer.h"

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
    char keyPressed = *imgWindowInteractor->GetKeySym();
    Lout(Logger::INFO) << "(char) keyPressed: " << keyPressed << endl;
    switch ( keyPressed ) {
    case keyMinusBrushSize:
      kv_opts->paintBrushRad = kv_opts->paintBrushRad-1;
      masterWindow->SetCircleCursorSize(kv_opts->paintBrushRad);
      break;
    case keyPlusBrushSize:
      kv_opts->paintBrushRad = kv_opts->paintBrushRad+1;
      masterWindow->SetCircleCursorSize(kv_opts->paintBrushRad);
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


}
