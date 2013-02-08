#include <iostream>
#include "KSlice.h"

#include "KSandbox.h"
#include "KDataWarehouse.h"
#include "KSegmentorBase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"


//vtkCxxRevisionMacro(KSlice, "$Revision$"); //necessary?
vtkStandardNewMacro(KSlice); //for the new() macro

//----------------------------------------------------------------------------


KSlice::KSlice( ) {
    dataWarehouse= new KDataWarehouse();

    BrushRad=7;
    NumIts=50;
    CurrSlice=50; //make this so segmentation is non-trivial (no points initialized on level set)
    DistWeight=.3;
    m_bUseEdgeBased=0;
    contInit=0;
    initCorrectFlag=0; //should not run updates before this flag is set to 1
}


KSlice::~KSlice() {
  delete dataWarehouse;

}



void KSlice::PasteSlice(int toSlice){
    ToSlice=toSlice;
    vrcl::copySliceFromTo( dataWarehouse->LabelVol, FromSlice, ToSlice);
}

void KSlice::Initialize(){
    //set up the segmentor
    dataWarehouse->ksegmentor= new KSegmentor3D(ImageVol, LabelVol, contInit, CurrSlice, NumIts, DistWeight, BrushRad);
    dataWarehouse->ksegmentor->SetUseEdgeBasedEnergy( m_bUseEdgeBased );
    dataWarehouse->ksegmentor->SetDistanceWeight(DistWeight);
    initCorrectFlag=1; //initialization is complete

}

void KSlice::runUpdate(){
    if(initCorrectFlag==1){ //already initialized
        dataWarehouse->ksegmentor->SetCurrentSlice(CurrSlice);
        dataWarehouse->ksegmentor->Update2D();
    }
}

void KSlice::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "Fill in the print function!" << std::endl;
}


