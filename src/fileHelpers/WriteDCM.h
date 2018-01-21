#pragma once
#include <itkImage.h>
#include <string.h>
#include "itkImageSeriesWriter.h"

#include "Helpers/TypeDefinitions.h"


	template<typename itkImageType>
	void writeDCMSeries(typename itkImageType::Pointer itkImage, std::string _filename, std::string directory)
	{
		typedef itk::ImageSeriesWriter< itkImageType, ImageType2D>  WriterType;

		std::vector<std::string> filenames;
		int zSize = itkImage->GetLargestPossibleRegion().GetSize()[2];
		if (zSize > 1000)
		{
			filenames.push_back(directory + _filename);
		}
		else
		{
			for (int i = 1; i <= itkImage->GetLargestPossibleRegion().GetSize()[2]; i++)
			{
				std::string filename = directory;
				filename += _filename;
				filename += std::to_string(i);
				filename += ".dcm";

				filenames.push_back(filename);
			}
		}

		WriterType::Pointer writer = WriterType::New();
		writer->SetFileNames(filenames);
		//writer1->SetFileName("C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\outputBiasCorrection\\test.dcm");
		writer->SetInput(itkImage);
		writer->Update();
	}
