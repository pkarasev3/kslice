#include "Gao3DRSS.h"

int main(int argc, char** argv)
{
  if (argc != 7)
    {
      std::cerr<<"Parameters: inputImage labelImageName outputImage expectedVolume intensityHomo[0~1] lambda[0~1]\n";
      exit(-1);
    }

  std::string originalImageFileName(argv[1]);
  std::string labelImageFileName(argv[2]);
  std::string segmentedImageFileName(argv[3]);
  double expectedVolume = atof(argv[4]);
  double intensityHomogeneity = atof(argv[5]);
  double curvatureWeight = atof(argv[6]);

  double maxRunningTime = 10000;
  short labelValue = 1;

  typedef short PixelType;
  typedef CSFLSRobustStatSegmentor3DLabelMap< PixelType > gao3d_itkpixel;

  // read input image
  typedef gao3d_itkpixel::TImage Image_t;

  typedef itk::ImageFileReader< Image_t > ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(originalImageFileName.c_str());
  Image_t::Pointer img;
    
  try
    {
      reader->Update();
      img = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }



  // read input label image
  typedef gao3d_itkpixel::TLabelImage LabelImage_t;

  typedef itk::ImageFileReader< LabelImage_t > LabelImageReader_t;
  LabelImageReader_t::Pointer readerLabel = LabelImageReader_t::New();
  readerLabel->SetFileName(labelImageFileName.c_str());
  LabelImage_t::Pointer labelImg;
    
  try
    {
      readerLabel->Update();
      labelImg = readerLabel->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }



  // preprocess label map (labelImg, the naming is confusing.....)
  LabelImage_t::Pointer newLabelMap = preprocessLabelMap<LabelImage_t::PixelType>(labelImg, labelValue);


  // do seg
  gao3d_itkpixel seg;
  seg.setImage(img);

  seg.setNumIter(10000); // a large enough number, s.t. will not be stoped by this creteria.
  seg.setMaxVolume(expectedVolume);
  seg.setInputLabelImage(newLabelMap);

  seg.setMaxRunningTime(maxRunningTime);


  seg.setIntensityHomogeneity(intensityHomogeneity);
  seg.setCurvatureWeight(curvatureWeight/1.5);

  seg.Segmentation_rss();




  typedef itk::Image< short, 3 > MaskImageType;

  MaskImageType::Pointer finalMask = getFinalMask<float>(seg.mp_phi, labelValue, 2.0);
  finalMask->CopyInformation(img);

  typedef itk::ImageFileWriter< MaskImageType > WriterType;
  WriterType::Pointer outputWriter = WriterType::New();
  outputWriter->SetFileName(segmentedImageFileName.c_str());
  outputWriter->SetInput(finalMask);
  outputWriter->Update();
  
  try
    {
      outputWriter->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      raise(SIGABRT);   
    }

  
  return EXIT_SUCCESS;
}


