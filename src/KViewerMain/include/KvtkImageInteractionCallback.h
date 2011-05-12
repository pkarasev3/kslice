#ifndef KvtkImageInteractionCallback_H
#define KvtkImageInteractionCallback_H

#endif // KvtkImageInteractionCallback_H

#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "KViewerOptions.h"
#include "opencv2/core/core.hpp"

// forward declaration
class KViewer; 
class vtkLookupTable;

// The mouse motion callback, to pick the image and recover pixel values
class KvtkImageInteractionCallback : public vtkCommand
{
public:
  static KvtkImageInteractionCallback *New()  {
    return new KvtkImageInteractionCallback;
  }

  KvtkImageInteractionCallback()  {
    this->Window     = NULL;
  }

  ~KvtkImageInteractionCallback()  {
    this->Window     = NULL;
  }

  void SetRenderWindow(vtkRenderWindow* Window)  {
    this->Window = Window;
  }
  void SetOptions(cv::Ptr<KViewerOptions> kv_opts)  {
    this->kv_opts = kv_opts;
  }

  void SetSaturationLookupTable( vtkLookupTable* satLUT ) {
    this->satLUT_shared = satLUT;
  }

  /** \brief handle the mouse interface for Kviewer VTK window
    * update the paint brush size, send update info to somewhere
    * TODO: is the way that this interacts with data thought out?
    * Right now it only communicates via KViewerOptions, is that OK?
    */
  void Execute(vtkObject *, unsigned long event, void *);

  void SetKViewerHandle( KViewer* kviewer ) { masterWindow = kviewer; 
                                              buttonDown = false; 
                                              erase = false; }

  bool ButtonDown() { return buttonDown; }
  
  bool Erase() { return erase; }
  
private:
  bool buttonDown;
  bool erase;
  int indexSliceCopyFrom;
  int indexSlicePasteTo;
  vtkRenderWindow*          Window;        // I don't own it, someone else cleans up
  vtkLookupTable*           satLUT_shared; // I don't own it, someone else cleans up
  KViewer*                  masterWindow;  // reverse handle on my source
  cv::Ptr<KViewerOptions>   kv_opts;       // not sure who owns it, be paranoid

};
