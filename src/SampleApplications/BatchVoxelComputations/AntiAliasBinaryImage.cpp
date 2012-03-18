
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"

typedef itk::Image<unsigned char, 2>  UnsignedCharImageType;
typedef itk::Image<float, 2>  FloatImageType;

static void CreateImage(UnsignedCharImageType::Pointer image);

int main(int argc, char* argv [])
{
    // Software Guide : BeginCodeSnippet
      const     unsigned int    Dimension = 3;

      typedef   unsigned short  InputPixelType;
      typedef   float           InternalPixelType;

      typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
      typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
    // Software Guide : EndCodeSnippet

      typedef   unsigned short   OutputPixelType;

      typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;


      typedef itk::ImageFileReader< InputImageType  >  ReaderType;

      ReaderType::Pointer reader = ReaderType::New();

      reader->SetFileName( argv[1] );

      try
        {
        reader->Update();
        }
      catch( itk::ExceptionObject & excep )
        {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
        }




  // Take the absolute value of the image
  typedef itk::AntiAliasBinaryImageFilter <InputImageType, InternalImageType>
          AntiAliasBinaryImageFilterType;

  AntiAliasBinaryImageFilterType::Pointer antiAliasFilter
          = AntiAliasBinaryImageFilterType::New ();
  antiAliasFilter->SetInput(reader->GetOutput());
  antiAliasFilter->Update();


  typedef itk::MultiplyByConstantImageFilter<InternalImageType,float, InternalImageType> MultiplyImageFilterType;
   // пизда с ушами antialias output sign...
    MultiplyImageFilterType::Pointer multiplyImageFilter = MultiplyImageFilterType::New();
    multiplyImageFilter->SetInput(antiAliasFilter->GetOutput());
    multiplyImageFilter->SetConstant(-1.0);
    multiplyImageFilter->Update();


  typedef itk::CastImageFilter< InternalImageType, OutputImageType > CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput(multiplyImageFilter->GetOutput());
  castFilter->Update();

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( argv[2] );
  writer->SetInput( castFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }



//  QuickView viewer;
//  viewer.AddImage<UnsignedCharImageType>(image);
//  viewer.AddImage<FloatImageType>(antiAliasFilter->GetOutput());
//  viewer.Visualize();

  return EXIT_SUCCESS;
}
