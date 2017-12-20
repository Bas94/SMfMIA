#pragma once
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

namespace Denoising 
{	
	vtkSmartPointer<vtkImageData> bilateralFilter(vtkSmartPointer<vtkImageData> imageData, double domainSigma, double rangeSigma);

	template<typename itkImageType>
	typename itkImageType::Pointer bilateralFilter2D(typename itkImageType::Pointer itkImagePointer, double domainSigma, double rangeSigma)
	{
		typedef itk::BilateralImageFilter<itkImageType, itkImageType >	BilateralImageFilter2D;
		BilateralImageFilter2D::Pointer bilateralFilter = BilateralImageFilter2D::New();
		bilateralFilter->SetInput(itkImagePointer);
		bilateralFilter->SetDomainSigma(domainSigma);
		bilateralFilter->SetRangeSigma(rangeSigma);
		bilateralFilter->Update();

		itkImageType::Pointer outputImage = bilateralFilter->GetOutput();
		return outputImage;
	}
}