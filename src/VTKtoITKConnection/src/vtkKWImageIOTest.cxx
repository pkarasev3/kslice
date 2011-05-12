/*=========================================================================

  Program:   KWImage - Kitware Image IO Library
  Module:    $RCSfile: vtkKWImageIOTest.cxx,v $

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

  if( argc < 2 )
    {
    std::cerr << "Missing filename command line argument" << std::endl;
    std::cerr << "Usage: " 
              << argv[0]
              << " inputFilename [expectedPixelType]" << std::endl;
    return EXIT_FAILURE;
    }

  // Create the pointer that will hold the image.
  vtkKWImage * kwImage = NULL;

  vtkKWImageIO * reader = vtkKWImageIO::New();
  
  reader->SetFileName( argv[1] );

  try
    {
    reader->ReadImage();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  kwImage = reader->HarvestReadImage();

  // Delete the reader to verify that the kwImage is self-suficient
  reader->Delete(); 

  vtkImageData * vtkImage = kwImage->GetVTKImage();

  int pixelType = vtkImage->GetScalarType();

  std::cout << "PixelType = " << pixelType << std::endl;

  vtkImage->Print( std::cout );
    
  kwImage->Delete();

  if( argc > 2 )
    {
    int expectedPixelType  = atoi( argv[2] );
    if( pixelType != expectedPixelType )
      {
      std::cerr << "PixelType does not correspond!" << std::endl;
      std::cerr << "Expected = " << expectedPixelType << std::endl;
      std::cerr << "Found    = " << pixelType << std::endl;
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}
