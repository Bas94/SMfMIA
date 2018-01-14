#include "BiasCorrection.h"
#include <itkImage.h>
//#include <itkBilateralImageFilter.h>
//#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkArray.h>

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

#include "itkResampleImageFilter.h"
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <algorithm>    // std::min_element, std::max_element

#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"

#include <itkStatisticsImageFilter.h>

namespace BiasCorrection
{

	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, 
		itk::Array<double> means, itk::Array<double> sigmas, double scalingFactor, bool scaling)
	{
		//convert imageData from VTK to ITK image
		ImageType::Pointer img = Converter::ConvertVTKToITK<ImageType>(imageData);
		ImageType::Pointer imgMask = Converter::ConvertVTKToITK<ImageType>(maskData);

		//Estimate the tissue values means and sigmas inside the mask
		//itk::ImageRegionIterator<ImageType> imageIterator(img, imgMask->GetRequestedRegion());
		//double summedGreyvalues = 0;
		//int countedPixel = 0;
		//unsigned char val = 0;

		//while (!imageIterator.IsAtEnd())
		//{
		//	// Get the value of the current pixel
		//	//unsigned char val = imageIterator.Get();
		//	//std::cout << (int)val << std::endl;

		//	// Set the current pixel to white
		//	// Get the value of the current pixel
		//	val = imageIterator.Get();
		//	//std::cout << (int)val << std::endl;
		//	summedGreyvalues = summedGreyvalues + (int)val;
		//	++countedPixel;

		//	++imageIterator;
		//}
		//double meanValue = summedGreyvalues / countedPixel;
		//std::cout << "Sum of Greyvalues: " << summedGreyvalues << std::endl;
		//std::cout << "Number of passed pixels: " << countedPixel << std::endl;
		//std::cout << "Mean Value: " << meanValue << std::endl;


		//typedef itk::StatisticsImageFilter<ImageType> StatisticsImageFilterType;
		//StatisticsImageFilterType::Pointer statisticsImageFilter
		//	= StatisticsImageFilterType::New();
		//statisticsImageFilter->SetInput(img);
		//statisticsImageFilter->Update();

		//std::cout << "Statistic over all voxels " << std::endl;
		//std::cout << "Mean: " << statisticsImageFilter->GetMean() << std::endl;
		//std::cout << "Std.: " << statisticsImageFilter->GetSigma() << std::endl;
		//std::cout << "Min: " << statisticsImageFilter->GetMinimum() << std::endl;
		//std::cout << "Max: " << statisticsImageFilter->GetMaximum() << std::endl;

		//Estimate the tissue values means and sigmas inside the mask
		typedef itk::LabelStatisticsImageFilter< ImageType, ImageType > LabelStatisticsImageFilterType;
		LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter = LabelStatisticsImageFilterType::New();
		labelStatisticsImageFilter->SetLabelInput(imgMask);
		labelStatisticsImageFilter->SetInput(img);
		labelStatisticsImageFilter->Update();

		std::cout << "Statistic over masked voxels " << std::endl;
		std::cout << "Number of labels: " << labelStatisticsImageFilter->GetNumberOfLabels() << std::endl;
		std::cout << std::endl;

		typedef LabelStatisticsImageFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
		typedef LabelStatisticsImageFilterType::LabelPixelType                LabelPixelType;

		double maximumMeanValue = 0;
		LabelPixelType wantedLabel = 0;


		for (ValidLabelValuesType::const_iterator vIt = labelStatisticsImageFilter->GetValidLabelValues().begin();
			vIt != labelStatisticsImageFilter->GetValidLabelValues().end();
			++vIt)
		{
			if (labelStatisticsImageFilter->HasLabel(*vIt))
			{
				LabelPixelType labelValue = *vIt;
				std::cout << "min: " << labelStatisticsImageFilter->GetMinimum(labelValue) << std::endl;
				std::cout << "max: " << labelStatisticsImageFilter->GetMaximum(labelValue) << std::endl;
				std::cout << "median: " << labelStatisticsImageFilter->GetMedian(labelValue) << std::endl;
				std::cout << "mean: " << labelStatisticsImageFilter->GetMean(labelValue) << std::endl;
				std::cout << "sigma: " << labelStatisticsImageFilter->GetSigma(labelValue) << std::endl;
				std::cout << "variance: " << labelStatisticsImageFilter->GetVariance(labelValue) << std::endl;
				std::cout << "sum: " << labelStatisticsImageFilter->GetSum(labelValue) << std::endl;
				std::cout << "count: " << labelStatisticsImageFilter->GetCount(labelValue) << std::endl;
				//std::cout << "box: " << labelStatisticsImageFilter->GetBoundingBox( labelValue ) << std::endl; // can't output a box
				std::cout << "region: " << labelStatisticsImageFilter->GetRegion(labelValue) << std::endl;
				std::cout << std::endl << std::endl;

				/*Since the usage of contrast agent, the tissue or label with a higher mean value should
				be the wanted, representing the enhanced liver tissue.*/
				if (labelStatisticsImageFilter->GetMean(labelValue) > maximumMeanValue)
				{
					maximumMeanValue = labelStatisticsImageFilter->GetMean(labelValue);
					wantedLabel = labelValue;
				}
			}
		}
		means[0] = labelStatisticsImageFilter->GetMean(wantedLabel);
		sigmas[0] = labelStatisticsImageFilter->GetSigma(wantedLabel);

		std::cout << "Used Mean Value for Bias Correction: " << means[0] << std::endl;
		std::cout << "Used Sigma Value for Bias Correction: " << sigmas[0] << std::endl;


		//Calculate wanted output size and output spacing for usage of resampling Filter.
		ImageType::SizeType inputSize = img->GetLargestPossibleRegion().GetSize();
		itk::Vector<double> inputSpacing = img->GetSpacing();
		ImageType::SizeType outputSize;
		ImageType::SpacingType outputSpacing;
		/*
		There exist two ways to reduce the amount of data points. 
		The first way is scaling, which reduces the number of datapoints by a scaling factor.
		e.g. scalingFactor = 2 --> structure with [20 42 24] points will be scaled to [10 21 12]
		The second way is respacing, especially to obtain an isotropic pixel spacing so that
		each voxel will be a cube with all sides of equal length.
		*/
		if (scaling == true) //first option, sample reduction by scaling
		{
			std::cout << "Start resampling by scaling with scaling factor of " << scalingFactor << std::endl;
			
			outputSize[0] = inputSize[0] / scalingFactor;
			outputSize[1] = inputSize[1] / scalingFactor;
			outputSize[2] = inputSize[2] / scalingFactor;

			std::cout << "Input size:" << inputSize << std::endl;
			std::cout << "Output size:" << outputSize << std::endl;

			//calculate Spacing
			outputSpacing[0] = inputSpacing[0] * (inputSize[0] / outputSize[0]);
			outputSpacing[1] = inputSpacing[1] * (inputSize[1] / outputSize[1]);
			outputSpacing[2] = inputSpacing[2] * (inputSize[2] / outputSize[2]);
			
			std::cout << "Input spacing: " << inputSpacing << std::endl;
			std::cout << "Output spacing: " << outputSpacing << std::endl;

			//std::cout << "Absolute size of original data: " << inputSize[0] * inputSpacing[0] << ' ' << inputSize[1] * inputSpacing[1] << ' ' << inputSize[2] * inputSpacing[2] << std::endl;
			//std::cout << "Absolute size of resampled data: " << outputSize[0] * outputSpacing[0] << ' ' << outputSize[1] * outputSpacing[1] << ' ' << outputSize[2] * outputSpacing[2] << std::endl;

		}
		else //second option, sample reduction by isotropic spacing
		{
			std::cout << "Start resampling by isotropic Voxel Spacing." << std::endl;

			// Find the largest spacing (mostly spacing of 3D image slice thickness)
			int numberOfElements = inputSpacing.Length; // establish size of array
			float maximumSpacing = inputSpacing[0];     // start with max = first element
			for (int i = 1; i < numberOfElements; i++)
			{
				if (inputSpacing[i] > maximumSpacing)
					maximumSpacing = inputSpacing[i];
			}
			std::cout << "Maximum value of input spacing: " << maximumSpacing << std::endl;

			outputSpacing[0] = maximumSpacing;
			outputSpacing[1] = maximumSpacing;
			outputSpacing[2] = maximumSpacing;

			std::cout << "Input spacing: " << inputSpacing << std::endl;
			std::cout << "Output spacing: " << outputSpacing << std::endl;

			//calculate Size
			outputSize[0] = inputSize[0] * (inputSpacing[0] / outputSpacing[0]);
			outputSize[1] = inputSize[1] * (inputSpacing[1] / outputSpacing[1]);
			outputSize[2] = inputSize[2] * (inputSpacing[2] / outputSpacing[2]);

			std::cout << "Input size: " << inputSize << std::endl;
			std::cout << "Output size: " << outputSize << std::endl;

			//std::cout << "Absolute size of original data: " << inputSize[0] * inputSpacing[0] << ' ' << inputSize[1] * inputSpacing[1] << ' ' << inputSize[2] * inputSpacing[2] << std::endl;
			//std::cout << "Absolute size of resampled data: " << outputSize[0] * outputSpacing[0] << ' ' << outputSize[1] * outputSpacing[1] << ' ' << outputSize[2] * outputSpacing[2] << std::endl;
		}

		//resampling of image data
		itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampleFilterForImage =
			itk::ResampleImageFilter<ImageType, ImageType>::New();
		resampleFilterForImage->SetInput( img );
		resampleFilterForImage->SetSize(outputSize);
		resampleFilterForImage->SetOutputSpacing( outputSpacing );
		resampleFilterForImage->SetTransform( itk::IdentityTransform<double,3>::New() );
		resampleFilterForImage->UpdateLargestPossibleRegion();
        ImageType::Pointer resampledImage = resampleFilterForImage->GetOutput();
        std::cout << "Resampling of image data has finished." << std::endl;

		//resampling of mask data
		itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampleFilterForMask =
			itk::ResampleImageFilter<ImageType, ImageType>::New();
		resampleFilterForMask->SetInput(imgMask);
		resampleFilterForMask->SetSize(outputSize);
		resampleFilterForMask->SetOutputSpacing(outputSpacing);
		resampleFilterForMask->SetTransform(itk::IdentityTransform<double, 3>::New());
		resampleFilterForMask->SetInterpolator(itk::NearestNeighborInterpolateImageFunction<ImageType, double>::New() );
		resampleFilterForMask->UpdateLargestPossibleRegion();
		ImageType::Pointer resampledMask = resampleFilterForMask->GetOutput();
		std::cout << "Resampling of mask data has finished." << std::endl;

		// Konsolenausgabe resampling-Ergebnis des Bildes
		//std::cout << "Image Output size: " << resampledImage->GetLargestPossibleRegion().GetSize() << std::endl;
		//std::cout << "Image Output spacing: " << resampledImage->GetSpacing() << std::endl;

		// Konsolenausgabe resampling-Ergebnis der Maske
		//std::cout << "Mask Output size: " << resampledMask->GetLargestPossibleRegion().GetSize() << std::endl;
		//std::cout << "Mask Output spacing: " << resampledMask->GetSpacing() << std::endl;


		std::cout << "Start BiasFieldCorrection." << std::endl;
		typedef itk::MRIBiasFieldCorrectionFilter< ImageType, ImageType, ImageType > ShadingFilterType;
		ShadingFilterType::Pointer shadingFilter = ShadingFilterType::New();
		shadingFilter->SetInput( resampledImage );
		shadingFilter->SetInputMask( resampledMask );
		shadingFilter->SetTissueClassStatistics(means, sigmas);
		shadingFilter->Update();
		shadingFilter->GetOutput();
		std::cout << "Biasfield estimation on downsampled data has finished. Begin Correction of original image data" << endl;

		//use old biasfield coefficients from downsampled bias filtering for usage on image data of original size.
		ShadingFilterType::Pointer shadingFilterOriginal = ShadingFilterType::New();
		shadingFilterOriginal->SetInput( img );
		shadingFilterOriginal->SetInputMask( imgMask );
		shadingFilterOriginal->SetTissueClassStatistics(means, sigmas);
		shadingFilterOriginal->Initialize();
		shadingFilterOriginal->SetInitialBiasFieldCoefficients(shadingFilter->GetEstimatedBiasFieldCoefficients() );
		//shadingFilterOriginal->SetInterSliceCorrectionMaximumIteration(100);
		//shadingFilterOriginal->SetVolumeCorrectionMaximumIteration(100);

		shadingFilterOriginal->Update();

        std::cout << "finished BiasFiledCorrection" << std::endl;
		vtkSmartPointer<vtkImageData> correctedImg = Converter::ConvertITKToVTK<ImageType>(shadingFilterOriginal->GetOutput());
		return correctedImg;

	}

}

