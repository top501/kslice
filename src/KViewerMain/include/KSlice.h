
#ifndef KVIEWER_GUI_H
#define KVIEWER_GUI_H


#include "KDataWarehouse.h"
#include "KViewerOptions.h"

void test_KSlice(); /** standalone test function */

class vtkKSlice : public vtkObject
{



public:
  static vtkKSlice* New();
  //vtkTypeRevisionMacro(vtkImageSlicePaint,vtkObject); if we implement this later
  void PrintSelf(ostream &os, vtkIndent indent);

  vtkSetObjectMacro(ImageVol, vtkImageData); //void SetImage(vtkImageData* image);
  vtkSetObjectMacro(LabelVol, vtkImageData); //void SetLabel(vtkImageData* label);
  vtkSetObjectMacro(UIVol, vtkImageData);    //void SetUI(vtkImageData* uiVol);

  vtkSetMacro(NumIts, int);               //void SetNumIters(int numIts);
  vtkSetMacro(BrushRad, int);             //void SetBrushRad(int rad);
  vtkSetMacro(CurrSlice, int);            //void SetCurrSlice(int currSlice);
  vtkSetMacro(DistWeight, float);         //void SetDistWeight(float distWeight);
  vtkSetMacro(FromSlice, int);            //void CopySlice(int fromSlice);



  //implemented functionality
  void PasteSlice(int toSlice);
  void Initialize();
  void runUpdate();

protected:
 vtkKSlice();
  ~vtkKSlice();
  KDataWarehouse* dataWarehouse; //all storage arrays
  //KViewerOptions* ksliceOptions; //app parameters, variables
  bool initCorrectFlag; //if Kslice has been initialized =1, else starts out being 0
  bool m_bUseEdgeBased; //if =1, use edge based energy, else region based (default =0)
  bool contInit; //flag for initializing the
private:
  vtkImageData* ImageVol;
  vtkImageData* LabelVol;
  vtkImageData* UIVol;
  int NumIts;
  int BrushRad;
  int CurrSlice;
  int DistWeight;
  int FromSlice;
  int ToSlice;


};
#endif