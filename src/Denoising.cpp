#include "Denoising.h"
#include <itkImage.h>
#include <itkBilateralImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkSmartPointer.h>

#include "Converter.h"

namespace Denoising 
{
	
	vtkSmartPointer<vtkImageData> bilateralFilter(vtkSmartPointer<vtkImageData> imageData, double domainSigma, double rangeSigma)
	{
		//// Setup types
		//const unsigned int Dimension = 3;

		typedef short PixelType;
		typedef itk::Image< PixelType, Dimension > ImageType;

		typedef itk::VTKImageToImageFilter< ImageType > VTKTOITKFilterType;
		VTKTOITKFilterType::Pointer filter = VTKTOITKFilterType::New();
		filter->SetInput(imageData);

		try
		{
			filter->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			std::cerr << "Error: " << error << std::endl;			
		}
		itk::VTKImageToImageFilter< ImageType >::Pointer  itkImage = ConvertVTKToITK(imageData);
		itk::Image<PixelType, Dimension>* itkImage2 = filter->GetOutput();

		typedef itk::BilateralImageFilter<ImageType, ImageType >	BilateralFilterType;
		BilateralFilterType::Pointer bilateralFilter = BilateralFilterType::New();
		bilateralFilter->SetInput(itkImage2);
		bilateralFilter->SetDomainSigma(domainSigma);
		bilateralFilter->SetRangeSigma(rangeSigma);
		bilateralFilter->Update();

		typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;
		ConnectorType::Pointer connector = ConnectorType::New();
		
		connector->SetInput(bilateralFilter->GetOutput());
		connector->Update();

		vtkSmartPointer<vtkImageData> smoothedImageData = vtkSmartPointer<vtkImageData>::New();
		smoothedImageData->DeepCopy(connector->GetOutput());

		return smoothedImageData;
	}
}
