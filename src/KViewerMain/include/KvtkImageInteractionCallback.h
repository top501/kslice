#ifndef KvtkImageInteractionCallback_H
#define KvtkImageInteractionCallback_H
#endif

#include <memory>
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "KViewerOptions.h"
#include <vtkWeakPointer.h>

// forward declaration
class KViewerParameterWidget;
class KViewer;
class vtkLookupTable;

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
  
  void SetOptions(std::shared_ptr<KViewerOptions> arg);

  void SetSaturationLookupTable( vtkLookupTable* satLUT ) ;

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

  void notifyChangeBrushSize( size_t k );

private:
  bool buttonDown;
  bool erase;
  int indexSliceCopyFrom;
  int indexSlicePasteTo;
  vtkRenderWindow*          Window;        // I don't own it, someone else cleans up
  vtkWeakPointer<vtkLookupTable>           satLUT_shared; // I don't own it, someone else cleans up
  KViewer*                  masterWindow;  // reverse handle on my source
  std::shared_ptr<KViewerOptions>   kv_opts;      
  std::unique_ptr<KViewerParameterWidget> m_paramWidget;
};