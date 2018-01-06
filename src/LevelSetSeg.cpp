#include "LevelSetSeg.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

namespace LevelSetSeg
{
	vtkSmartPointer<vtkImageData> levelSetSegmentation(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkImageData> imageMask, const double propagationScaling, const double numberOfIterations)
	{
		InputImageType::Pointer img = InputImageType::New();
		void * pimg = &img;
		Converter::ConvertVTKToITK(pimg, imageData);

		InputImageType::Pointer msk = InputImageType::New();
		void * pmsk = &msk;
		Converter::ConvertVTKToITK(pmsk, imageData);

		typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType, InputImageType >  GeodesicActiveContourFilterType;
		GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
		geodesicActiveContour->SetPropagationScaling(propagationScaling);
		geodesicActiveContour->SetCurvatureScaling(1.0);
		geodesicActiveContour->SetAdvectionScaling(1.0);
		geodesicActiveContour->SetMaximumRMSError(0.02);
		geodesicActiveContour->SetNumberOfIterations(numberOfIterations);
		geodesicActiveContour->SetInput(img);
		geodesicActiveContour->SetFeatureImage(msk);
		//geodesicActiveContour->Update();

		typedef itk::BinaryThresholdImageFilter< InputImageType, ImageType > ThresholdingFilterType;
		ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
		thresholder->SetLowerThreshold(-1000.0);
		thresholder->SetUpperThreshold(0.0);
		thresholder->SetOutsideValue(itk::NumericTraits< OutputPixelType >::min());
		thresholder->SetInsideValue(itk::NumericTraits< OutputPixelType >::max());
		thresholder->SetInput(geodesicActiveContour->GetOutput());
		
		thresholder->Update();

		
		ImageType::Pointer newImage = thresholder->GetOutput();
		
		CastFilterType::Pointer caster = CastFilterType::New();
		WriterType::Pointer writer = WriterType::New();
		
		caster->SetInput(thresholder->GetOutput());
		writer->SetInput(caster->GetOutput());
		caster->SetOutputMinimum(0);
		caster->SetOutputMaximum(255);
		writer->SetFileName("GeodesicActiveContourImageFilterOutput1.png");
		//writer->Update();
		typedef itk::ImageToVTKImageFilter<ImageType>            ConnectorTypeLevelSet;

		ConnectorTypeLevelSet::Pointer connector = ConnectorTypeLevelSet::New();

		connector->SetInput(caster->GetOutput());
		connector->Update();

		vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
		vtkImage->DeepCopy(connector->GetOutput());

		return vtkImage;
	}
}