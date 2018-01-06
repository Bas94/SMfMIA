#pragma once
#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkSmartPointer.h>
#include <itkVTKImageToImageFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkVTKImageToImageFilter.h>

#include "Helpers/TypeDefinitions.h"

namespace Converter
{
	
    template<typename itkImageType>
    typename itkImageType::Pointer ConvertVTKToITK( vtkSmartPointer<vtkImageData> imageData )
    {
        typedef itk::VTKImageToImageFilter<itkImageType> VTKTOITKFilterType;
        typename VTKTOITKFilterType::Pointer filter = VTKTOITKFilterType::New();
        filter->SetInput(imageData);
        try
        {
            filter->Update();
        }
        catch (itk::ExceptionObject & error)
        {
            std::cerr << "Error: " << error << std::endl;
        }

        return filter->GetOutput();
    }

	template<typename itkImageType>
	vtkSmartPointer<vtkImageData> ConvertITKToVTK(typename itkImageType::Pointer itkImagePointer)
	{
        typename itk::ImageToVTKImageFilter<itkImageType>::Pointer connector = itk::ImageToVTKImageFilter<itkImageType>::New();

		connector->SetInput(itkImagePointer);
		connector->Update();

		vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
		vtkImage->DeepCopy(connector->GetOutput());

		return vtkImage;
	}
}
