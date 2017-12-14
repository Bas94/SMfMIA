#include "BiasCorrection.h"
#include <itkImage.h>
//#include <itkBilateralImageFilter.h>
//#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkArray.h>

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

#include "itkResampleImageFilter.h"

namespace BiasCorrection
{

	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, itk::Array<double> means, itk::Array<double> sigmas)
	{
		//convert imageData from VTK to ITK image
        ImageType::Pointer img = Converter::ConvertVTKToITK<ImageType>( imageData );

        double scale = 2;
        std::cout << "start resampling" << std::endl;
        itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampleFilter =
                itk::ResampleImageFilter<ImageType, ImageType>::New();
        ImageType::SizeType inputSize = img->GetLargestPossibleRegion().GetSize();
        ImageType::SizeType outputSize;
        outputSize[0] = inputSize[0] / scale;
        outputSize[1] = inputSize[1] / scale;
        outputSize[2] = inputSize[2] / scale;
        ImageType::SpacingType outputSpacing;
        outputSpacing[0] = img->GetSpacing()[0] * scale;
        outputSpacing[1] = img->GetSpacing()[1] * scale;
        outputSpacing[2] = img->GetSpacing()[2] * scale;
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

#if 0
        shadingFilter->Initialize();
        ShadingFilterType::BiasFieldType biasfield =
            shadingFilter->EstimateBiasField( resampledImage->GetLargestPossibleRegion(),
                                              shadingFilter->GetBiasFieldDegree(),
                                              shadingFilter->GetVolumeCorrectionMaximumIteration() );


        // convert ITK to VTK image
        //ImageType::Pointer image = shadingFilter->GetOutput();
        shadingFilter->SetInput( img );
        shadingFilter->SetInitialBiasFieldCoefficients( biasfield.GetCoefficients() );
        shadingFilter->Initialize();
        shadingFilter->SetVolumeCorrectionMaximumIteration( 1 );
        shadingFilter->SetInterSliceCorrectionMaximumIteration( 1 );
#endif

        std::cout << "finished BiasFiledCorrection" << std::endl;
        return Converter::ConvertITKToVTK<ImageType>(shadingFilter->GetOutput());
	}
}

