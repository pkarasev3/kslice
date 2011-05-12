#ifndef GAO3DRSS_H_
#define GAO3DRSS_H_

// #pragma once Don't use me... not supported everywhere/always, an MSVC thing I hear  #pragma once

#include <iostream>
#include "signal.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "labelMapPreprocessor.h"
#include "SFLSRobustStatSegmentor3DLabelMap_single.h"
#include "SFLSRobustStatSegmentor3DLabelMap_single.txx"
#include "SFLSSegmentor3D.txx"


typedef short PixelType;
typedef CSFLSRobustStatSegmentor3DLabelMap< PixelType > gao3d_itkpixel;
typedef gao3d_itkpixel::TImage Image_t;
typedef gao3d_itkpixel::TLabelImage LabelImage_t;
typedef itk::Image< short, 3 > MaskImageType;

struct Gao3DOptions  {
  Gao3DOptions();
  double expectedVolume;
  double intensityHomogeneity;
  double curvatureWeight;
  double maxRunningTime;
  short  labelValue;
  int    numIters;

  Image_t::Pointer  img;
  LabelImage_t::Pointer label;
};

void Gao3DRSS_about();

class Gao3DRSS
{
// TODO: Gao3DRSS should inherit from a baseclass (e.g. the KSegmentor), then the viewer selects a segmentor by dynamic pointer cast
// TODO: something is weird, templated function time inside of the class? That's what overloading in .cpp is for no?
public:
  Gao3DRSS( );
  Gao3DRSS( const Gao3DOptions& opts ) { Initialize( opts.img, opts.label, opts.expectedVolume,
                  opts.intensityHomogeneity, opts.curvatureWeight, opts.maxRunningTime,
                  opts.labelValue ); }

	template< typename TPixel >itk::Image< short, 3 >::Pointer getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, unsigned char l, TPixel thod);

	double GetExpectedVolume();
	void SetExpectedVolume(double expectedVolume);
	double GetIntensityHomogeneity();
	void SetIntensityHomogeneity(double intensityHomogeneity);
	double GetCurvatureWeight();
	void SetCurvatureWeight(double curvatureWeight);
	double GetMaxRunningTime();
	void SetMaxRunningTime(double maxRunningTime);
	short GetLabelValue();
	void SetLabelValue(short labelValue);

  /** \returns  equivalent to returning itk::Image< short, 3 >::Pointer
    */
	MaskImageType::Pointer PerformSegmentation(int numIters);

protected:
	void Initialize(Image_t::Pointer img, LabelImage_t::Pointer label, double expectedVolume, double intensityHomogeneity, double curvatureWeight, double maxRunningTime, short labelValue);

  double expectedVolume;
  double intensityHomogeneity;
  double curvatureWeight;

  double maxRunningTime;
  short labelValue;

  Image_t::Pointer img; //equivalent to itk::Image<TPixel, 3>::Pointer img;

  /** TODO: use doxy tags if you're taking time to make a comment
    * \brief  initial user selected pixels
    */
  LabelImage_t::Pointer label; //equivalent to itk::Image<float, 3> label;


  LabelImage_t::Pointer newLabelMap; //equivalent to itk::Image<float, 3> label;

};

#endif
