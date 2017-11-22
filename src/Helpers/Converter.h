#pragma once
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkSmartPointer.h>
#include <itkVTKImageToImageFilter.h>

#include "Helpers\TypeDefinitions.h"
namespace Converter
{

	typedef short PixelTypeConv;
	typedef itk::Image< PixelTypeConv, 3 > ImageTypeConv;


	void ConvertVTKToITK(void * pimg, vtkSmartPointer<vtkImageData> imageData);

	//itk::Image<PixelTypeConv, 3>::Pointer itkImageConv;
}
