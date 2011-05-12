#include "vtkProgrammableSource.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkSmartPointer.h"
#include "KSandbox.h"

#include "Logger.h"
#include "KSegmentor.h"
#include "algorithm"
#include "vector"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LevelSetCurve.h"

using std::vector;
using std::cout;
using std::endl;
using namespace vrcl;
using namespace cv;

namespace {
  const static string PrintVerbose = "VerboseKSegUtils";
}

namespace  vrcl  
{

  void waterMark(const std::string& text, Mat & img)
  {
    int baseline = 0;
    Size textSize = getTextSize(text, CV_FONT_HERSHEY_PLAIN, 3, 2, &baseline);
    Point textOrg((img.cols - textSize.width) / 2, (img.rows + textSize.height) / 2);
    putText(img, text, textOrg, CV_FONT_HERSHEY_PLAIN, 3, Scalar::all(1), 2, 8);
  
  }


  void copySliceFromTo( vtkImageData* label_map, int idxFrom, int idxTo )
{
    short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
    int dims[3];
    label_map->GetDimensions( dims );
    assert( idxFrom >= 0 && idxTo >= 0 && idxFrom < dims[2] && idxTo < dims[2] );
    int num_changed = 0;
    for (int i = 0; i < dims[0]; i++)      {
      for (int j = 0; j < dims[1]; j++)    {
            long elemNumFrom = idxFrom * dims[0] * dims[1] + j * dims[1] + i;
            long elemNumTo   = idxTo   * dims[0] * dims[1] + j * dims[1] + i;
            short prevVal    = ptrLabel[elemNumTo];
            short newVal     = ptrLabel[elemNumFrom];
            ptrLabel[elemNumTo] = newVal;
            num_changed  +=  ( newVal != prevVal );
      }
    }
    cout << "copy/paste from " << idxFrom << " to "
         << idxTo << " changed # pixels: " << num_changed << endl;
}


void getVolumeAsString( const vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 string & volumeString )
{
    short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
    int dims[3];
    label_map->GetDimensions( dims );
    double area_sum = 0.0;
    double label_range[2];
    label_map->GetScalarRange( label_range );

    for (int k = 0; k < dims[2]; k++)    {
      for (int i = 0; i < dims[0]; i++)      {
        for (int j = 0; j < dims[1]; j++)    {
                long elemNum = k * dims[0] * dims[1] + j * dims[1] + i;
                area_sum    += 1.0*(1e-6 < double( ptrLabel[elemNum] ) ) ;
        }
      }
    }
    area_sum /= pow(10.0, 3.0); // 1mL = 1cm^3 = 1000mm^3
    area_sum *= (imageSpacing[0] * imageSpacing[1] * imageSpacing[2]);
    std::stringstream ss;
    ss << "  Volume: " << area_sum << " mL ";
    volumeString = ss.str();
    IFLOG( PrintVerbose, cout << "label range: "<<label_range[0]
                              << ","<<label_range[1]<< endl )
    IFLOG( PrintVerbose, cout << "image spacing: " << cv::Mat( imageSpacing ) )
    IFLOG( PrintVerbose, cout << volumeString )
}


} // end namespace





#if 0   // Firing Squad


template <class IT>
  void copy_ptr_filter(IT* inPtr, IT* outPtr, int size ) {
  for(int i=0; i<size; i++) { // 'demo' operation: just copy!
    outPtr[i] = inPtr[i];
  }
}

/** Call the basic 'growing' method from kwidget 2D left
 */
void KSegment_basic_grow();

/** Call the basic 'shrinking' method from kwidget 2D left
 */
void KSegment_basic_shrink();

void KWidget_2D_left::KSegment_glob3D_grow( )  {
  kv_data->labelDataArray_new = vtkSmartPointer<vtkImageData>::New();
  kv_data->labelDataArray_new->ShallowCopy( kv_data->labelDataArray );
  kv_data->labelDataArray_new = Grow3DGlob( kv_data->labelDataArray );
  kv_data->labelDataArray_new->Update();
  label2D_shifter_scaler->SetInput( kv_data->labelDataArray_new );
  label2D_shifter_scaler->Update();
  check_extents(kv_data->labelDataArray_new);
  kv_data->labelDataArray = kv_data->labelDataArray_new;
}

void KWidget_2D_left::KSegment_glob3D_shrink( ) {
  kv_data->labelDataArray_new = vtkSmartPointer<vtkImageData>::New();
  kv_data->labelDataArray_new->ShallowCopy( kv_data->labelDataArray );
  kv_data->labelDataArray_new = Shrink3DGlob( kv_data->labelDataArray );
  kv_data->labelDataArray_new->Update();
  label2D_shifter_scaler->SetInput( kv_data->labelDataArray_new );
  label2D_shifter_scaler->Update();
  check_extents(kv_data->labelDataArray_new);
  kv_data->labelDataArray = kv_data->labelDataArray_new;
}


void KViewer::KSegment_basic_grow() {
    kwidget_2d_left->KSegment_glob3D_grow(); // widget updates itself
    kwidget_3d_right->UpdateVolumeRenderer( kv_data->imageVolumeRaw, kv_data->labelDataArray );
    UpdateModel3D();
}

void KViewer::KSegment_basic_shrink() {
  kwidget_2d_left->KSegment_glob3D_shrink(); // widget updates itself
  kwidget_3d_right->UpdateVolumeRenderer( kv_data->imageVolumeRaw, kv_data->labelDataArray );
  qVTK1->update();
  qVTK2->update();
  UpdateModel3D();
} 

void interactiveMeanSeparation( const Mat& img, const Mat& phi, Mat& result, int threadKey )
{ 
  // This is accidentally multithreaded so you can segment multiple slices at once...
  // uh... let's take advantage of that by giving windows unique names? 
 
  std::stringstream ss;
  ss << "Segmenting Slice " << " ... Esc or 'q' to return to main interface";
  string window_name = ss.str();
  cout << ss << " " << threadKey << endl;
  namedWindow( window_name,  WINDOW_AUTOSIZE);
  imshow(window_name, img); 
  waitKey(1);
  LevelSetCurve  LS( img, phi, string( window_name ) );
  char c = 'c';
  IplImage output   = IplImage(img);
  cv::Mat imgToShow = img.clone();
  cout<<"running a few iterations per key stroke... hit 'q' or 'Esc' to return to main interface " << endl;
  while( c != 'q' && c != '\x1b') {
    int its = 30;
    LS.EvolveSparseField( its, &output );
    cout << " . " ;
  
    // waterMark("press 'q' to return to main app",imgToShow);
    cvShowImage(window_name.c_str(), &output);
    c = waitKey(0);
  }
  cout << endl;
  cout << "returning to kviewer_desktop " << endl;
  LS.getPhi(result);

  
}

void runMeanSeparationOnSlice( vtkImageData* image, vtkImageData* label, int sliceIndex )
{
  short *ptrLabel = static_cast<short*>(label->GetScalarPointer());
  short *ptrImage = static_cast<short*>(image->GetScalarPointer());
  int dimsLabel[3];
  int dimsImage[3];
  label->GetDimensions( dimsLabel );
  image->GetDimensions( dimsImage );
  int k = sliceIndex;
  double imgRange[2], labelRange[2];
  image->GetScalarRange( imgRange );
  label->GetScalarRange( labelRange );

  Mat img = cv::Mat( dimsImage[0], dimsImage[1], CV_32FC3 );
  Mat phi = cv::Mat( dimsLabel[0], dimsLabel[1], CV_32F );
  assert( phi.size() == img.size() &&  k < dimsLabel[2] );

  for (int i = 0; i < dimsLabel[0]; i++)      {
    for (int j = 0; j < dimsLabel[1]; j++)    {
          long elemNum       = k * dimsLabel[0] * dimsLabel[1] + j * dimsLabel[1] + i;
          short imgVal       = ptrImage[elemNum];
          short phiVal       = ptrLabel[elemNum];
          img.at<float>(i,j) = float( imgVal );
          phi.at<float>(i,j) = float( phiVal > 0 );
      }
  }
  phi  = (-phi+0.5)*2.5; // negative inside, positive outside
  img  = (img - imgRange[0])* (1.0 / (imgRange[1]-imgRange[0]) );
  Mat phi_new = Mat::zeros(phi.size(), phi.type() );
  interactiveMeanSeparation( img, phi, phi_new, sliceIndex ); // do the sfls work 
  for (int i = 0; i < dimsLabel[0]; i++)      {
    for (int j = 0; j < dimsLabel[1]; j++)    {
          float phival       = ( phi_new.at<float>(i,j) < 0 ) * 1.0;    
          long elemNum       = k * dimsLabel[0] * dimsLabel[1] + j * dimsLabel[1] + i;
          ptrLabel[elemNum]  = short( phival * labelRange[1] );     
     }
  }

}


vtkSmartPointer<vtkImageData> Grow3DGlob( vtkImageData* input ) {
  check_extents(input);
  vtkSmartPointer<vtkImageContinuousDilate3D> dilate_filter;
  dilate_filter = vtkSmartPointer<vtkImageContinuousDilate3D>::New();
  dilate_filter->SetKernelSize(3,3,5);
  dilate_filter->SetInput( input );
  vtkSmartPointer<vtkImageGaussianSmooth> gaussian_smoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
  gaussian_smoother->SetInputConnection( dilate_filter->GetOutputPort( ) );
  gaussian_smoother->SetStandardDeviations( 0.25, 0.25, 1.0 );
  gaussian_smoother->Update();
  vtkSmartPointer<vtkImageData> output = vtkSmartPointer<vtkImageData>::New();
  output = dilate_filter->GetOutput();
  check_extents(output);
  return output;
}

vtkSmartPointer<vtkImageData> Shrink3DGlob(   vtkImageData* input ) {
  check_extents(input);
  vtkSmartPointer<vtkImageContinuousErode3D> erode_filter;
  erode_filter = vtkSmartPointer<vtkImageContinuousErode3D>::New();
  erode_filter->SetKernelSize(3,3,5);
  erode_filter->SetInput( input );
  vtkSmartPointer<vtkImageGaussianSmooth> gaussian_smoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
  gaussian_smoother->SetInputConnection( erode_filter->GetOutputPort( ) );
  gaussian_smoother->SetStandardDeviations( 0.25, 0.25, 1.0 );
  gaussian_smoother->Update();
  vtkSmartPointer<vtkImageData> output = vtkSmartPointer<vtkImageData>::New();
  output = erode_filter->GetOutput();
  check_extents(output);
  return output;
}

#endif
