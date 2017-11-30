#include "BiasCorrection.h"
#include <itkImage.h>
//#include <itkBilateralImageFilter.h>
//#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkArray.h>

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

namespace BiasCorrection
{

	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, itk::Array<double> means, itk::Array<double> sigmas)
	{
		//convert imageData from VTK to ITK image
		ImageType::Pointer img = ImageType::New();
		void * pimg = &img;
		Converter::ConvertVTKToITK(pimg, imageData);


		//convert maskData from VTK to ITK image
		ImageType::Pointer mask = ImageType::New();
		void * pmask = &mask;
		Converter::ConvertVTKToITK(pmask, maskData);


		//typedef itk::N4BiasFieldCorrectionImageFilter< ImageType, ImageType, ImageType > ShadingFilterType;
		//ShadingFilterType::Pointer shadingFilter = ShadingFilterType::New();
		//shadingFilter->SetInput1(filter1->GetOutput()); //give first input
		//shadingFilter->SetInput2(filter2->GetOutput()); //give Masks
		//itk::Array<int> MaxNumIteration(1);
		//MaxNumIteration.SetElement(0, 10);
		//shadingFilter->SetMaximumNumberOfIterations(MaxNumIteration);
		//shadingFilter->SetInput2(filter->GetOutput()); //give second input
		//shadingFilter->Update();

		typedef itk::MRIBiasFieldCorrectionFilter< ImageType, ImageType, ImageType > ShadingFilterType;
		ShadingFilterType::Pointer shadingFilter = ShadingFilterType::New();
		shadingFilter->SetInput(img);
		shadingFilter->SetTissueClassStatistics(means, sigmas);
		shadingFilter->SetInputMask(mask);
		shadingFilter->GetOutputMask();

		shadingFilter->Update();


		// convert ITK to VTK image
		ImageType::Pointer image = shadingFilter->GetOutput();

		return Converter::ConvertITKToVTK(image);
	}
}
