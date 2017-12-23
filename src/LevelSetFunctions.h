#pragma once

#include <itkImage.h>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <itkBinaryContourImageFilter.h>
namespace LevelSet
{
	const unsigned int                Dimension = 2;
	typedef float                                    InputPixelType;
	typedef itk::Image< InputPixelType, Dimension >  InputImageType;
	typedef unsigned char                            OutputPixelType;
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

	typedef  itk::ImageFileReader< InputImageType >  ReaderType;
	typedef  itk::ImageFileWriter< OutputImageType > WriterType;

	void runLevelSet2D(const std::string inputFileName, const std::string inputMaskFileName, const std::string outputFileName, const std::string outputDirectory);

	template<typename itkImageType>
	std::vector<int> computeMeanValueOfMask(typename itkImageType::Pointer mask)
	{
		int width = mask->GetLargestPossibleRegion().GetSize()[0];
		int height = mask->GetLargestPossibleRegion().GetSize()[1];
		int nPixelsMask = 0;
		MaskType2D::IndexType index;
		int mean_x = 0, mean_y = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				index[0] = x;
				index[1] = y;
				bool maskIsSet = static_cast<bool>(mask->GetPixel(index));
				if (maskIsSet)
				{
					mean_x += x;
					mean_y += y;
					nPixelsMask++;
				}
			}
		}
		mean_x = mean_x / nPixelsMask;
		mean_y = mean_y / nPixelsMask;

		std::vector<int> outputValues;
		outputValues.push_back(mean_x);
		outputValues.push_back(mean_y);

		return outputValues;
	}

	template<typename itkImageType>
	double distanceConturToSeedPoint(typename itkImageType::Pointer mask, std::vector<int> seedPoint)
	{
		itk::BinaryContourImageFilter<itkImageType, itkImageType>::Pointer contourFilter =
			itk::BinaryContourImageFilter<itkImageType, itkImageType>::New();

		contourFilter->SetInput(mask);
		contourFilter->SetBackgroundValue(0);
		contourFilter->SetForegroundValue(1);
		contourFilter->SetFullyConnected(true);
		contourFilter->Update();


		int width = mask->GetLargestPossibleRegion().GetSize()[0];
		int height = mask->GetLargestPossibleRegion().GetSize()[1];
		int nPixelContour = 0;
		MaskType2D::IndexType index;
		int mean_d = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				index[0] = x;
				index[1] = y;
				bool maskIsSet = static_cast<bool>(mask->GetPixel(index));
				if (maskIsSet)
				{
					// calculate euclidean distance
					double distance = sqrt((x*seedPoint[0]) + (y*seedPoint[1]));
					mean_d += distance;
					nPixelContour++;
				}
			}
		}
		mean_d = ((1.0)*mean_d) / nPixelContour;
		return mean_d;
	}
}