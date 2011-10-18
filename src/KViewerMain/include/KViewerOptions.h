#ifndef KVIEWEROPTIONS_H_
#define KVIEWEROPTIONS_H_

#include <string>
#include <vector>
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"

struct KViewerOptions
{
public:
  static double getDefaultDrawLabelMaxVal() {
    return 1000.0;
  }

public:
  int     numSlices;
  int     imgHeight;
  int     imgWidth;
  double  sliceZSpace;
  double  sliderMax;
  double  sliderMin;
  int     paintBrushRad;
  double  paintBrushThreshold; 
  int     loadImageTrigger;

  //vtkSmartPointer<vtkMetaImageReader> labelFileReader;

  std::vector<std::string> LabelArrayFilenames;
  std::string ImageArrayFilename;

  float modelOpacity3D;
  float labelOpacity2D;
  bool  labelInterpolate;
  bool  writeCompressed;
  int   minIntensity;
  int   maxIntensity;

  double drawLabelMaxVal; // value of "inside" labelmap
  int    segmentor_iters; // # of curve evolution update iterations
  int    multilabel_paste_mode; // copy/paste, do all labels, or only active, or ?
  int    multilabel_sgmnt_mode; // run segmentor, do all labels, or only active, or ?

//////////////////////

  // BAD  (these are "State" not "Options" )
  std::vector<double> imageOrigin;
  std::vector<double> imageSpacing;
  std::vector<int>    imageExtent;

  /////////////////////////

  
  // METHODS

  /** send args from main() to me to set params
    */
  void setFromArgs(int argc, char **argv);

  /** print help about args
    */
  void help();

void LoadImage( );

void LoadLabel(const std::string& loadLabel);

void PrintLogo( );

int GetBrushSize();



};





#endif

