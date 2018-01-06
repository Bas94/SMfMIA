#pragma once

#include "Helpers/Converter.h"
#include "Helpers/TypeDefinitions.h"

namespace LevelSetSeg
{
	vtkSmartPointer<vtkImageData> levelSetSegmentation(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> imageMask, const double propagationScaling, const double numberOfIterations);
}