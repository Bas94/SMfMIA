#pragma once
#include <itkImage.h>

const unsigned int Dimension = 3;

typedef short PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

typedef itk::VTKImageToImageFilter< ImageType > VTKTOITKFilterType;