#ifndef KvtkImageInteractionCallback_H
#define KvtkImageInteractionCallback_H
#endif

#include <memory>
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "KViewerOptions.h"
#include <vtkWeakPointer.h>
#include <map>

// forward declaration
class KViewerParameterWidget;
class KViewer;
class vtkLookupTable;

namespace vrcl {
    class KSegmentorBase;
}

// The mouse motion callback, to pick the image and recover pixel values
class KVIEWER_EXPORT KvtkImageInteractionCallback : public vtkCommand
{
public:
  static KvtkImageInteractionCallback *New();

  KvtkImageInteractionCallback();

  ~KvtkImageInteractionCallback();

  void SetRenderWindow(vtkRenderWindow* Window)  {
    this->Window = Window;
  }

  void notifyAllFromOptions(std::shared_ptr<KViewerOptions> arg);

  void notifyViewDir(bool r0, bool r1, bool r2); // mutually exclusive + left to right precedence order
  
  void setViewDir(bool rotX, bool rotY, bool rotZ);

  void SetOptions(std::shared_ptr<KViewerOptions> arg);

  void SetSaturationLookupTable( vtkLookupTable* satLUT ) ;

  /** \brief handle the mouse interface for Kviewer VTK window    
    */
  void Execute(vtkObject *, unsigned long event, void *);

  void SetKViewerHandle( KViewer* kviewer ) { masterWindow = kviewer;
                                              buttonDown = false;
                                              erase = false; }

  bool ButtonDown() { return buttonDown; }

  bool Erase() { return erase; }

  void notifyChangeBrushSize( size_t k );

private:
  bool buttonDown;
  bool erase;
  int indexSliceCopyFrom;
  int indexSlicePasteTo;
  bool m_bInsideViewCallback;
  vtkRenderWindow*          Window;        // I don't own it, someone else cleans up
  vtkWeakPointer<vtkLookupTable>           satLUT_shared; // I don't own it, someone else cleans up
  KViewer*                  masterWindow;  // reverse handle on my source
  std::shared_ptr<KViewerOptions>   kv_opts;      
  std::unique_ptr<KViewerParameterWidget> m_paramWidget;

  std::map<vrcl::KSegmentorBase*,std::weak_ptr<vrcl::KSegmentorBase>> m_segmentors;
};
