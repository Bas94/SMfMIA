#include "Denoising.h"
#include <itkImage.h>
#include <itkBilateralImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkSmartPointer.h>

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

namespace Denoising
{
	
	vtkSmartPointer<vtkImageData> bilateralFilter(vtkSmartPointer<vtkImageData> imageData, double domainSigma, double rangeSigma)
	{
		ImageType::Pointer img = ImageType::New();
		void * pimg = &img;
		Converter::ConvertVTKToITK(pimg,imageData);

		typedef itk::BilateralImageFilter<ImageType, ImageType >	BilateralFilterType;
		BilateralFilterType::Pointer bilateralFilter = BilateralFilterType::New();
		bilateralFilter->SetInput(img);
		bilateralFilter->SetDomainSigma(domainSigma);
		bilateralFilter->SetRangeSigma(rangeSigma);
		bilateralFilter->Update();
		
		ImageType::Pointer image = bilateralFilter->GetOutput();		

		return Converter::ConvertITKToVTK(image);
	}
}
