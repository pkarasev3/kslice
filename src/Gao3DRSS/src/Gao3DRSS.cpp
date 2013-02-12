#include "Gao3DRSS.h"
#include "Logger.h"
#include <iostream>
#include "signal.h"

#include "SFLSRobustStatSegmentor3DLabelMap_single.h"
#include "labelMapPreprocessor.h"
#include "itkImageFileWriter.h"

using namespace vrcl;

Gao3DOptions::Gao3DOptions() {
  // set some default values, TODO: these could be declared const somewhere for clarity
  expectedVolume        =10;
  intensityHomogeneity  =.5;
  curvatureWeight       =.2;
  maxRunningTime        =1;
  labelValue            =1;
  numIters              =200;

  img   = NULL;
  label = NULL;
  SETLOG("Gao3D_WriteAlways",0)
  SETLOG("Gao3D_TimingDebug",1)
}

void Gao3DRSS_about() {
  LOUT "Gao3DRSS library successfully links..."  LENDL
    }

void Gao3DRSS::Initialize(Image_t::Pointer img, LabelImage_t::Pointer label,
                          double expectedVolume, double intensityHomogeneity, double curvatureWeight,
                          double maxRunningTime, short labelValue)
{
  //initialize internal parameters
  this->expectedVolume = expectedVolume;
  this->intensityHomogeneity = intensityHomogeneity;
  this->curvatureWeight = curvatureWeight;
  this->maxRunningTime = maxRunningTime;
  this->labelValue = labelValue;
  this->img=img;
  this->label=label;

  // preprocess label map
  this->newLabelMap = preprocessLabelMap<LabelImage_t::PixelType>(label, labelValue);


  IFLOG("Gao3D_WriteAlways", {
    // non-obvious operation that paradoxically does NOT have a comment #1
    typedef itk::ImageFileWriter< Image_t > WriterType1;
    WriterType1::Pointer outputWriter1 = WriterType1::New();
    outputWriter1->SetFileName("BeforeSeg_image.mha");
    outputWriter1->SetInput(img);
    outputWriter1->Update();
    outputWriter1->Write();

    // non-obvious operation that paradoxically does NOT have a comment #2
    typedef itk::ImageFileWriter< LabelImage_t > WriterType2;
    WriterType2::Pointer outputWriter2 = WriterType2::New();
    outputWriter2->SetFileName("BeforeSeg_label.mha");
    outputWriter2->SetInput(label);
    outputWriter2->Update();
    outputWriter2->Write();
  } )
}

namespace {
  enum Segmentation_Mode{RSS, CHAN_VESE, BHATTACHARYYA};
}

MaskImageType::Pointer Gao3DRSS::PerformSegmentation(int numIters)
{
  cout<<"Running Gao3DRSS::PerformSegmentation" << endl;
  //perform segmentation
  gao3d_itkpixel seg;

  seg.setImage(this->img);
  seg.setNumIter(numIters);
  seg.setMaxVolume(this->expectedVolume);
  seg.setInputLabelImage(this->newLabelMap);
  seg.setMaxRunningTime(this->maxRunningTime);
  seg.setIntensityHomogeneity(this->intensityHomogeneity);
  seg.setCurvatureWeight(this->curvatureWeight);

  Segmentation_Mode  gao3d_mode = CHAN_VESE;
  switch( gao3d_mode) {
  case RSS:
    seg.Segmentation_rss();
    break;
  case CHAN_VESE:
    seg.Segmentation_chan_vese();
    break;
  case BHATTACHARYYA:
    seg.Segmentation_bhattacharyya();
    break;
  default:
      throw "bogus Segmentation_Mode!";
  }

  MaskImageType::Pointer finalMask = getFinalMask<float>(seg.mp_phi, this->labelValue, 0.0);
  finalMask->CopyInformation(this->img);

  IFLOG("Gao3D_WriteAlways",
    typedef itk::ImageFileWriter< MaskImageType > WriterType;
    WriterType::Pointer outputWriter = WriterType::New();
    outputWriter->SetFileName("AfterSeg_mask.mha");
    outputWriter->SetInput(finalMask);
    outputWriter->Update();
  )

return finalMask;
}



template< typename TPixel >itk::Image< short, 3 >::Pointer Gao3DRSS::getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, unsigned char l, TPixel thod)
{
  typedef itk::Image< short, 3 > MaskType;

  MaskType::SizeType size = img->GetLargestPossibleRegion().GetSize();

  long nx = size[0];
  long ny = size[1];
  long nz = size[2];


  MaskType::Pointer mask = MaskType::New();
  MaskType::IndexType start = {{0, 0, 0}};

  MaskType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  mask->SetRegions( region );

  mask->SetSpacing(img->GetSpacing());
  mask->SetOrigin(img->GetOrigin());

  mask->Allocate();
  mask->FillBuffer(0);


  for (long ix = 0; ix < nx; ++ix)
  {
    for (long iy = 0; iy < ny; ++iy)
    {
      for (long iz = 0; iz < nz; ++iz)
      {
        MaskType::IndexType idx = {{ix, iy, iz}};
        TPixel v = img->GetPixel(idx);

        mask->SetPixel(idx, v<=thod?l:0);
      }
    }
  }



  return mask;
}


double Gao3DRSS::GetExpectedVolume() {
  return this->expectedVolume;
}

void Gao3DRSS::SetExpectedVolume(double expectedVolume) {
  this->expectedVolume=expectedVolume;
}

double Gao3DRSS::GetIntensityHomogeneity() {
  return this->intensityHomogeneity;
}

void Gao3DRSS::SetIntensityHomogeneity(double intensityHomogeneity) {
  this->intensityHomogeneity=intensityHomogeneity;
}

double Gao3DRSS::GetCurvatureWeight() {
  return this->curvatureWeight;
}

void Gao3DRSS::SetCurvatureWeight(double curvatureWeight) {
  this->curvatureWeight=curvatureWeight;
}

double Gao3DRSS::GetMaxRunningTime() {
  return this->maxRunningTime;
}

void Gao3DRSS::SetMaxRunningTime(double maxRunningTime) {
  this->maxRunningTime=maxRunningTime;
}

short Gao3DRSS::GetLabelValue() {
  return this->labelValue;
}

void Gao3DRSS::SetLabelValue(short labelValue) {
  this->labelValue=labelValue;
}





















