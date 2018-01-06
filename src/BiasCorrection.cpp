#include "BiasCorrection.h"
#include <itkImage.h>
//#include <itkBilateralImageFilter.h>
//#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkArray.h>

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

#include "itkResampleImageFilter.h"
#include <algorithm>    // std::min_element, std::max_element

namespace BiasCorrection
{

	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, itk::Array<double> means, itk::Array<double> sigmas)
	{
		//convert imageData from VTK to ITK image
		ImageType::Pointer img = Converter::ConvertVTKToITK<ImageType>(imageData);

		//The scaling value will describe the factor, by which the amounth
		//of data points will be changed for each direction.
		//e.g. scale = 2 --> structure with [20 42 24] points will be scaled to [10 21 12]
		double scale = 2;
		std::cout << "start resampling" << std::endl;
		itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampleFilter =
			itk::ResampleImageFilter<ImageType, ImageType>::New();
		ImageType::SizeType inputSize = img->GetLargestPossibleRegion().GetSize();
		ImageType::SizeType outputSize;
		outputSize[0] = inputSize[0] / scale;
		outputSize[1] = inputSize[1] / scale;
		outputSize[2] = inputSize[2] / scale;

		std::cout << "inputSize:" << inputSize << std::endl;
		std::cout << "outputSize:" << outputSize << std::endl;

		//for maximum 'highspeed' we need uniformly shaped voxels.
		//therefore choose the one with highest spacing and resample 
		//other dimensions to this spacing.
		itk::Vector<double> inputSpacing = img->GetSpacing();
		std::cout << "inputSpacing:" << inputSpacing << std::endl;
		int numberOfElements = inputSpacing.Length; // establish size of array
		float max = inputSpacing[0];       // start with max = first element
		for (int i = 1; i < numberOfElements; i++)
		{
			if (inputSpacing[i] > max)
				max = inputSpacing[i];
		}
		::cout << "maximum of inputSpacing:" << max << std::endl;

		ImageType::SpacingType outputSpacing;
		//evtl scale entfernen weil flascher sachverhalt..?
		outputSpacing[0] = max * scale;
		outputSpacing[1] = max * scale;
		outputSpacing[2] = max * scale;
		std::cout << "outputSpacing:" << outputSpacing << std::endl;

		std::cout << "absoluteSizeOfOriginalData:" << inputSize[0] * inputSpacing[0] << ' ' << inputSize[1] * inputSpacing[1] << ' ' << inputSize[2] * inputSpacing[2] << std::endl;
		std::cout << "absoluteSizeOfResampledData:" << outputSize[0] * outputSpacing[0] <<' '<< outputSize[1] * outputSpacing[1] << ' ' << outputSize[2] * outputSpacing[2] << std::endl;

        resampleFilter->SetSize( outputSize );
        resampleFilter->SetInput( img );
        resampleFilter->SetOutputSpacing( outputSpacing );
        resampleFilter->SetTransform( itk::IdentityTransform<double,3>::New() );
        resampleFilter->UpdateLargestPossibleRegion();
        ImageType::Pointer resampledImage = resampleFilter->GetOutput();
        std::cout << "resampling finished" << std::endl;

		//convert maskData from VTK to ITK image
        //ImageType::Pointer mask = Converter::ConvertVTKToITK<ImageType>( maskData );

        std::cout << "start BiasFiledCorrection" << std::endl;
        typedef itk::MRIBiasFieldCorrectionFilter< ImageType, ImageType, ImageType > ShadingFilterType;
        ShadingFilterType::Pointer shadingFilter = ShadingFilterType::New();
        shadingFilter->SetInput( resampledImage );
        shadingFilter->SetTissueClassStatistics(means, sigmas);
        shadingFilter->SetBiasFieldMultiplicative( true );
        shadingFilter->GetOutputMask();
		shadingFilter->Update(); //vermutlich unnötig

//#if 0
//        shadingFilter->Initialize();
//        ShadingFilterType::BiasFieldType biasfield =
//            shadingFilter->EstimateBiasField( resampledImage->GetLargestPossibleRegion(),
//                                              shadingFilter->GetBiasFieldDegree(),
//                                              shadingFilter->GetVolumeCorrectionMaximumIteration() );
//
//
//        // convert ITK to VTK image
//        //ImageType::Pointer image = shadingFilter->GetOutput();
//        shadingFilter->SetInput( img );
//        shadingFilter->SetInitialBiasFieldCoefficients( biasfield.GetCoefficients() );
//        shadingFilter->Initialize();
//        shadingFilter->SetVolumeCorrectionMaximumIteration( 1 );
//        shadingFilter->SetInterSliceCorrectionMaximumIteration( 1 );
//#endif

        std::cout << "finished BiasFiledCorrection" << std::endl;
		vtkSmartPointer<vtkImageData> correctedImg = Converter::ConvertITKToVTK<ImageType>(shadingFilter->GetOutput());
		return correctedImg;
	}
}

