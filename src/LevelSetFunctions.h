#pragma once

#include <itkImage.h>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <itkBinaryContourImageFilter.h>
#include "Helpers/TypeDefinitions.h"

namespace LevelSet
{
	typedef float                                    InputPixelType2D;
	typedef itk::Image< InputPixelType2D, 2 >  InputImageType2D;
	typedef unsigned char                            OutputPixelType2D;
	typedef itk::Image< OutputPixelType2D, 2 > OutputImageType2D;

	typedef  itk::ImageFileReader< InputImageType2D >  ReaderType2D;
	typedef  itk::ImageFileWriter< OutputImageType2D > WriterType2D;

	typedef float                                    InputPixelType3D;
	typedef short									OutputPixelType3D;
	typedef itk::Image< InputPixelType3D, 3 >  InputImageType3D;
	typedef itk::Image< OutputPixelType3D, 3 > OutputImageType3D;

	typedef  itk::ImageFileReader< InputImageType3D >  ReaderType3D;
	typedef  itk::ImageFileWriter< OutputImageType3D > WriterType3D;
		
	OutputImageType2D::Pointer runLevelSet2D(const std::string inputFileName, LevelSet::ReaderType2D::Pointer readerMask, const std::string outputFileName, const std::string outputDirectory, const double sigma, const int alpha, const int beta,const double propagationScaling, const double curvaturScaling, const double advectionScaling, const double numberOfIterations);
	
	void runLevelSet3D(OutputImageType3D::Pointer itkImageData, const std::string outputFileName, const std::string outputDirectory);

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
		if (nPixelsMask != 0)
		{
			mean_x = mean_x / nPixelsMask;
			mean_y = mean_y / nPixelsMask;

		}
		std::vector<int> outputValues;
		outputValues.push_back(mean_x);
		outputValues.push_back(mean_y);

		return outputValues;
	};

	template<typename itkImageType>
	double distanceConturToSeedPoint(typename itkImageType::Pointer mask, std::vector<int> seedPoint)
	{
        typename itk::BinaryContourImageFilter<itkImageType, itkImageType>::Pointer contourFilter =
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
	};
}
