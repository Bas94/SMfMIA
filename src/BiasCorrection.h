
/* This function will correct the MRI bias field by using ITKs MRIBiasFieldCorrectionFilter.
* The Bias-field will be corrected by tissue statistics of mean greyvalue and sigma.
* These parameters are determined automatically using ITKs LabelStatisticsImageFilter.
* For faster Bias-Field estimation, a downscaling of data point amount is provided by using
* ITKs ResampleImageFilter.
* There exist two ways to reduce the amount of data points.
* The first way is scaling, which reduces the number of datapoints by a scaling factor.
* The second way is respacing, especially to obtain an isotropic pixel spacing so that
* each voxel will be cube-shaped with all sides of equal length.
* The Bias-field will first be estimated on downsampled data, after calculation the 
* bias coefficients are used for processing the original image data. For faster calculation
* reduce iteration numbers of single-slice and inter-slice bias-field estimation.
*
* Input:
* imageData - original 3D image data
* maskData - original 3D mask data, it is sufficient if the mask data is only a
* rough representation of a single tissue type.
* scalingFactor - describes the amount the data points will be reduced in each dimension.
* For bool scaling = false this factor won't be in use.
* scaling - set On/Off the type of data point reduction. If 'false', downsampling is provided
* by calculation isotropic voxal spacing using the widest spacing of original data.
* iterationsSingleSlice - maximum number of iterations for each image slice on original sized data 
* to correct bias-artefact
* iterationsSInterSlice - maximum number of iterations for inter-slice correction of bias-artefact
* on original sized data
* 
* Output:
* correctedImage - originally sized, corrected 3D image data 
*/

#pragma once
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <itkArray.h>

namespace BiasCorrection
{
	vtkSmartPointer<vtkImageData> shadingFilter(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> maskData, 
		double scalingFactor, bool scaling, int iterationsSingleSlice, int iterationsInterSlice);
}