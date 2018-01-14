
/* This function will correct the MRI bias field by using ITKs MRIBiasFieldCorrectionFilter.
* Give a range of expected greyvalues by mean and sigma, which represent the tissue inside the mask.
* If there is more than one tissue type expected inside the mask, then for each class define
* mean and sigma in an array (length == number of expected tissues).
* example of parameters for .../data/p05/1_int/10_data: mean=550, sigma=410
*/

#pragma once
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkArray.h>

namespace BiasCorrection
{
	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, itk::Array<double> means, itk::Array<double> sigmas, double scalingFactor, bool scaling);
}