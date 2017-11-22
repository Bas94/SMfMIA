#pragma once
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkBilateralImageFilter.h>

const unsigned int Dimension = 3;

typedef short PixelType;
typedef itk::Image< PixelType, Dimension > ImageType;

typedef itk::VTKImageToImageFilter< ImageType > VTKTOITKFilterType;

typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;

typedef itk::BilateralImageFilter<ImageType, ImageType >	BilateralFilterType;