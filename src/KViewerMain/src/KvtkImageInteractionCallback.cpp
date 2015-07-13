#include "KvtkImageInteractionCallback.h"
#include "ParameterWidget.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "vtkLookupTable.h"
#include "KViewer.h"
#include "KViewerOptions.h"
#include <QDebug>

using cv::Ptr;
using namespace vrcl;
using std::cout;
using std::endl;

namespace 
{
const char keyMinusBrushSize    ='[';
const char keyPlusBrushSize     =']';

const char keyCopyLabelSlice    ='c';
const char keyPasteLabelSlice   ='v';
const char keyPasteRangeSlices  ='V';

const char keyUpShiftSatRange   ='l'; // shift both min and max up
const char keyDownShiftSatRange ='k'; // shift both min and max down
const char keyUpMaxSatRange     ='j'; // shift *only max* up
const char keyDownMaxSatRange   ='J'; // shift *only max* down
const char keyDownMinSatRange   ='h'; // shift *only min* down
const char keyUpMinSatRange     ='H'; // shift *only min* up

const char keyUpLabelOpacity    ='p';
const char keyDownLabelOpacity  ='o';
}

class SignalBlocker
{
public:
  SignalBlocker(QObject *obj)
  {
    insert( QList<QObject*>()<<obj );
  }

  SignalBlocker(QList<QObject*>  objects)
  {
    insert(objects);
  }

  void insert(QList<QObject*>  objects)
  {
    for (auto obj : objects)
      m_objs.insert(obj, obj->signalsBlocked());
    blockAll();
  }

  void blockAll() {
    for( auto m_obj : m_objs.keys() )
      m_obj->blockSignals(true);
  }

  ~SignalBlocker()
  {
    for( auto m_obj : m_objs.keys() )
      m_obj->blockSignals( m_objs[m_obj] );
  }

private:
  QMap<QObject*,bool> m_objs;
  
};

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
    using namespace std::placeholders;
    this->Window = NULL;
    this->m_paramWidget.reset(new KViewerParameterWidget); // = std::make_unique<KViewerParameterWidget>();

    {
      auto cb = std::bind(&KvtkImageInteractionCallback::notifyAllFromOptions,this,_1);
      m_paramWidget->setOptionsUpdateCallback(cb);
    }
    {
      auto cb = std::bind(&KvtkImageInteractionCallback::notifyViewDir,this,_1,_2,_3);
      m_paramWidget->setViewDirUpdateCallback(cb);
    }
    m_bInsideViewCallback = false;
}

void KvtkImageInteractionCallback::SetOptions(std::shared_ptr<KViewerOptions> kv_opts)  {
    this->kv_opts = kv_opts;
    this->m_paramWidget->populateFromOptions( this->kv_opts );
}

void KvtkImageInteractionCallback::notifyChangeBrushSize(size_t k)
{
    masterWindow->SetCircleCursorSize(k);

    this->masterWindow->updatePaintBrushStatus( NULL );
}

void KvtkImageInteractionCallback::SetSaturationLookupTable(vtkLookupTable* lut)
{
    this->satLUT_shared = lut;
    this->m_paramWidget->setSaturationLUT(satLUT_shared);
}

void KvtkImageInteractionCallback::setViewDir(bool rotX, bool rotY, bool rotZ)
{  
  PRINT_AND_EVAL( QVector<int>()<<rotX<<rotY<<rotZ );
  SignalBlocker tmp( QList<QObject*>() 
    << m_paramWidget->radioButton_View0
    << m_paramWidget->radioButton_View1
    << m_paramWidget->radioButton_View2
    );
  m_paramWidget->radioButton_View1->setChecked( (rotX>0) );
  m_paramWidget->radioButton_View2->setChecked( (rotY>0) );
  m_paramWidget->radioButton_View0->setChecked( (rotY==0) && (rotX==0) );
  
}

void KvtkImageInteractionCallback::notifyViewDir(bool r0, bool r1, bool r2)
{
    if(m_bInsideViewCallback)
      return;
    
    m_bInsideViewCallback = true;
    if( ! (r0 || r1 || r2 ) )
      r0 = true;

    auto obj = this->Window->GetInteractor();
    char key = '\0';

    if (r0)
      key = 'z';

    if (r1)
      key = 'e';
    
    if (r2)
      key = 't';
    
    obj->SetKeyCode(key);
    PRINT_AND_EVAL(QString().sprintf("%d,%d,%d,",r0,r1,r2)+QString()<<key);
    this->masterWindow->handleGenericEvent(obj,0);
    this->m_bInsideViewCallback = false;
}

void KvtkImageInteractionCallback::notifyAllFromOptions( std::shared_ptr<KViewerOptions> opts )
{
    kv_opts = opts;
    notifyChangeBrushSize(kv_opts->paintBrushRad);

    {
        double* sat_range = satLUT_shared->GetRange();
        auto segmentors   = masterWindow->GetSegmentors();
        PRINT_AND_EVAL("updating params: "<<QVector<double>::fromStdVector({kv_opts->lambda,(double)kv_opts->rad,(double)kv_opts->segmentor_iters}));
        for( auto s : segmentors) 
        {
            s->SetSaturationRange(sat_range[0],sat_range[1]); 
            s->SetLambda(kv_opts->lambda);
            s->SetContRad(kv_opts->rad);
            s->setNumIterations(kv_opts->segmentor_iters);                        
        }
    }
    
    masterWindow->UpdateMultiLabelDisplay();
    masterWindow->update();    
}

void KvtkImageInteractionCallback::Execute(vtkObject *, unsigned long event, void *)
{

  if(event == vtkCommand::LeftButtonPressEvent )
  {    
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
    auto keyChar            = imgWindowInteractor->GetKeyCode();
    
    PRINT_AND_EVAL((char)keyPressed << keyChar << keyMinusBrushSize << (unsigned int)keyMinusBrushSize );
    if( (char)keyPressed == 'R' )
      keyChar = 'R';
    if( (char)keyPressed == 'L' ) 
      keyChar = 'L';

    bool bHandled = true;

    switch ( keyChar ) 
    {
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
      bHandled = false;
      break;
    }
    if( bUpdatedSatLut ) {
      cout << "updated satLUT range: " << satLUT_shared->GetTableRange()[0]
           << ", " << satLUT_shared->GetTableRange()[1] << endl;
    }
    PRINT_AND_EVAL(bHandled);
    if (bHandled)
      this->m_paramWidget->populateFromOptions(kv_opts);
  }

  // trigger the main window to update text displaying paintbrush info
  this->masterWindow->updatePaintBrushStatus( NULL );
    
  

}
