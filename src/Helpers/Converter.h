#pragma once
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkSmartPointer.h>
#include <itkVTKImageToImageFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "Helpers\TypeDefinitions.h"
namespace Converter
{
	
	void ConvertVTKToITK(void * pimg, vtkSmartPointer<vtkImageData> imageData);

	vtkSmartPointer<vtkImageData> ConvertITKToVTK(ImageType::Pointer itkImagePointer);
}
