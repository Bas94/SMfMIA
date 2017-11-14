#pragma once
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

namespace Denoising 
{
	vtkSmartPointer<vtkImageData> bilateralFilter(vtkSmartPointer<vtkImageData> imageData, double domainSigma, double rangeSigma);
}