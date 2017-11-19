#pragma once
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkSmartPointer.h>
#include <itkVTKImageToImageFilter.h>

typedef short PixelType;
const unsigned int Dimension = 3;

itk::VTKImageToImageFilter<itk::Image<PixelType, Dimension>>::Pointer  ConvertVTKToITK(vtkSmartPointer<vtkImageData> imageData);

itk::Image<PixelType, Dimension>::Pointer itkImageConv;

