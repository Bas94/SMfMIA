#pragma once

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

#include <vector>
namespace LevelSetSeg
{
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
}