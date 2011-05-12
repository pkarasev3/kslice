/*=========================================================================

  Program:   KWImage - Kitware Image IO Library
  Module:    $RCSfile: vtkKWImageIOTest2.cxx,v $

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkKWImageIO.h"
#include "itkImage.h"
#include "vtkImageData.h"
#include "vtkKWImage.h"


int main( int argc, char * argv [] )
{ // TODO: Put me into a "test" or "exe" Folder!

  if( argc < 3 )
    {
    std::cerr << "Missing filename command line argument" << std::endl;
    std::cerr << "Usage: " 
              << argv[0]
              << " inputFilename outputFilename" << std::endl;
    return EXIT_FAILURE;
    }

  vtkKWImageIO * reader = vtkKWImageIO::New();
  
  reader->SetFileName( argv[1] );

  std::cout << "Read the image first" << std::endl;
  try
    {
    reader->ReadImage();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  vtkKWImage  *  kwImage = NULL; 
  kwImage = reader->HarvestReadImage();

  vtkImageData  * vtkImage;

  vtkImage = kwImage->GetVTKImage();

  int expectedPixelType = vtkImage->GetScalarType();

  reader->Delete();
 
  vtkKWImageIO * writer = vtkKWImageIO::New();
  writer->SetFileName( argv[2] );
  writer->SetImageToBeWritten( kwImage );
  
  std::cout << "Save the image..." << std::endl;

  try
    {
    writer->WriteImage();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  kwImage->Delete();
  writer->Delete();
  
  // read the image again and verify that the 
  //  pixel type matches  

  vtkKWImageIO * reader2 = vtkKWImageIO::New();
  
  reader2->SetFileName( argv[2] );
  std::cout << "Read the image again" << std::endl;
  try
    {
    reader2->ReadImage();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  vtkKWImage    * kwImage2;   
  kwImage2 = reader2->HarvestReadImage();

  int pixelType =  kwImage2-> GetVTKImage()->GetScalarType();

  std::cout << "PixelType = " << pixelType << std::endl;

  kwImage2->Delete();
  reader2->Delete();

  if( pixelType != expectedPixelType )
    {
    std::cerr << "PixelType does not correspond!" << std::endl;
    std::cerr << "Expected = " << expectedPixelType << std::endl;
    std::cerr << "Found    = " << pixelType << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
