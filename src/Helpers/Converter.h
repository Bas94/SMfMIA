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


	vtkSmartPointer<vtkImageData> ConvertITKToVTK(ImageType::Pointer itkImagePointer);
}
