#pragma once
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkBilateralImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <opencv2/core.hpp>

const unsigned int Dimension = 3;

typedef short PixelType;
typedef itk::Image< PixelType, Dimension >               ImageType;
typedef itk::Image< PixelType, Dimension >               MaskType;
typedef itk::Image< float,     Dimension >               FloatImageType;

typedef itk::Image< PixelType, 2 >               ImageType2D;
typedef itk::Image< PixelType, 2 >               MaskType2D;
typedef itk::Image< float,     2 >               FloatImageType2D;

typedef itk::VTKImageToImageFilter<ImageType>            VTKTOITKFilterType;

typedef itk::ImageToVTKImageFilter<ImageType>            ConnectorType;

typedef itk::BilateralImageFilter<ImageType, ImageType > BilateralFilterType;
typedef std::vector<cv::Point2d>                         Contour;
