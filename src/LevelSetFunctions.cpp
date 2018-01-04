#include "LevelSetFunctions.h"

#include "Helpers/Converter.h"
#include "Denoising.h"
#include "SMfMIAImageViewer.h"
#include "LevelSetSeg.h"
#include "Helpers/Converter.h"

#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"


namespace LevelSet
{
	void runLevelSet2D(const std::string inputFileName, const std::string inputMaskFileName, const std::string outputFileName, const std::string outputDirectory)
	{
		const double sigma = 1.0;

		ReaderType2D::Pointer reader = ReaderType2D::New();
		reader->SetFileName(inputFileName);
		ReaderType2D::Pointer readerMask = ReaderType2D::New();
		readerMask->SetFileName(inputMaskFileName);
		readerMask->Update();

		InputImageType2D::Pointer bilateralSmoothedImage = Denoising::bilateralFilterTemplate<InputImageType2D>(reader->GetOutput(), 2, 100);
		typedef  itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType2D, InputImageType2D > GradientFilterType;
		GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
		gradientMagnitude->SetSigma(sigma);		//gradientMagnitude->SetInput(readerMask->GetOutput());
		gradientMagnitude->SetInput(bilateralSmoothedImage);

		//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType>(gradientMagnitude->GetOutput()));

		typedef  itk::SigmoidImageFilter< InputImageType2D, InputImageType2D > SigmoidFilterType2D;
		SigmoidFilterType2D::Pointer sigmoid = SigmoidFilterType2D::New();
		sigmoid->SetOutputMinimum(0.0);
		sigmoid->SetOutputMaximum(1.0);
		sigmoid->SetAlpha(25);
		sigmoid->SetBeta(100);
		sigmoid->SetInput(gradientMagnitude->GetOutput());
		//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType>(sigmoid->GetOutput()));
		//  The FastMarchingImageFilter requires the user to provide a seed
		//  point from which the level set will be generated. The user can actually
		//  pass not only one seed point but a set of them. Note the the
		//  FastMarchingImageFilter is used here only as a helper in the
		//  determination of an initial level set. We could have used the
		//  \doxygen{DanielssonDistanceMapImageFilter} in the same way.
		typedef  itk::FastMarchingImageFilter< InputImageType2D, InputImageType2D > FastMarchingFilterType;
		FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();

		typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType2D, InputImageType2D >  GeodesicActiveContourFilterType;
		GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
		geodesicActiveContour->SetPropagationScaling(3.0);
		geodesicActiveContour->SetCurvatureScaling(6.0);
		geodesicActiveContour->SetAdvectionScaling(10.0);
		geodesicActiveContour->SetMaximumRMSError(0.01);
		//geodesicActiveContour->SetReverseExpansionDirection(true);
		geodesicActiveContour->SetNumberOfIterations(3000);
		geodesicActiveContour->SetInput(fastMarching->GetOutput());
		geodesicActiveContour->SetFeatureImage(sigmoid->GetOutput());



		typedef itk::BinaryThresholdImageFilter< InputImageType2D, OutputImageType2D > ThresholdingFilterType;
		ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
		thresholder->SetLowerThreshold(-1000.0);
		thresholder->SetUpperThreshold(0.0);
		thresholder->SetOutsideValue(itk::NumericTraits< OutputPixelType2D >::min());
		thresholder->SetInsideValue(itk::NumericTraits< OutputPixelType2D >::max());
		thresholder->SetInput(geodesicActiveContour->GetOutput());

		typedef FastMarchingFilterType::NodeContainer  NodeContainer;
		typedef FastMarchingFilterType::NodeType       NodeType;

		/* TODO: calculate seed points with mask and inter_image
		for that calculate middlepoint pixel of image mask
		setze den setValue auf den Wert der Distanz zwischen seedPoint und wirklicher Kontur, dafuer muss aus
		der Maske die Kontur berechnet werden und der durchschnittliche Abstand von vorher berechnetem seedPoint
		zu Kontur berechnet werden
		*/
		//	Note that here we assign the value of minus the
		//  user-provided distance to the unique node of the seeds passed to the
		//  FastMarchingImageFilter. In this way, the value will increment
		//  as the front is propagated, until it reaches the zero value corresponding
		//  to the contour. After this, the front will continue propagating until it
		//  fills up the entire image. The initial distance is taken here from the
		//  command line arguments. The rule of thumb for the user is to select this
		//  value as the distance from the seed points at which she want the initial
		//  contour to be.
		std::vector<int> seedPoint = computeMeanValueOfMask<InputImageType2D>(readerMask->GetOutput());
		double mean_d = distanceConturToSeedPoint<InputImageType2D>(readerMask->GetOutput(), seedPoint);

		InputImageType2D::IndexType  seedPosition;
		seedPosition[0] = seedPoint[0];
		seedPosition[1] = seedPoint[1];

		NodeContainer::Pointer seeds = NodeContainer::New();
		NodeType node;
		node.SetValue(-5.0);
		node.SetIndex(seedPosition);

		seeds->Initialize();
		seeds->InsertElement(0, node);

		fastMarching->SetTrialPoints(seeds);
		fastMarching->SetSpeedConstant(1.0);

		typedef itk::RescaleIntensityImageFilter< InputImageType2D, OutputImageType2D > CastFilterType;

		CastFilterType::Pointer caster1 = CastFilterType::New();
		CastFilterType::Pointer caster2 = CastFilterType::New();
		CastFilterType::Pointer caster3 = CastFilterType::New();
		CastFilterType::Pointer caster4 = CastFilterType::New();

		WriterType2D::Pointer writer1 = WriterType2D::New();
		WriterType2D::Pointer writer2 = WriterType2D::New();
		WriterType2D::Pointer writer3 = WriterType2D::New();
		WriterType2D::Pointer writer4 = WriterType2D::New();

		//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType>(sigmoid->GetOutput()));

		caster1->SetInput(bilateralSmoothedImage);
		writer1->SetInput(caster1->GetOutput());
		writer1->SetFileName(outputDirectory + "GeodesicActiveContourImageFilterOutput1.dcm");
		caster1->SetOutputMinimum(itk::NumericTraits< OutputPixelType2D >::min());
		caster1->SetOutputMaximum(itk::NumericTraits< OutputPixelType2D >::max());
		writer1->Update();

		caster2->SetInput(gradientMagnitude->GetOutput());
		writer2->SetInput(caster2->GetOutput());
		writer2->SetFileName(outputDirectory + "GeodesicActiveContourImageFilterOutput2.dcm");
		caster2->SetOutputMinimum(itk::NumericTraits< OutputPixelType2D >::min());
		caster2->SetOutputMaximum(itk::NumericTraits< OutputPixelType2D >::max());
		writer2->Update();

		caster3->SetInput(sigmoid->GetOutput());
		writer3->SetInput(caster3->GetOutput());
		writer3->SetFileName(outputDirectory + "GeodesicActiveContourImageFilterOutput3.dcm");
		caster3->SetOutputMinimum(itk::NumericTraits< OutputPixelType2D >::min());
		caster3->SetOutputMaximum(itk::NumericTraits< OutputPixelType2D >::max());
		writer3->Update();

		caster4->SetInput(geodesicActiveContour->GetOutput());
		writer4->SetInput(caster4->GetOutput());
		writer4->SetFileName(outputDirectory + "GeodesicActiveContourImageFilterOutput4.dcm");
		caster4->SetOutputMinimum(itk::NumericTraits< OutputPixelType2D >::min());
		caster4->SetOutputMaximum(itk::NumericTraits< OutputPixelType2D >::max());

		fastMarching->SetOutputSize(
			reader->GetOutput()->GetBufferedRegion().GetSize());

		WriterType2D::Pointer writer = WriterType2D::New();
		writer->SetFileName(outputDirectory+outputFileName);
		writer->SetInput(thresholder->GetOutput());
		try
		{
			writer->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			std::cerr << "Error: " << error << std::endl;
		}

		std::cout << std::endl;
		std::cout << "Max. no. iterations: " << geodesicActiveContour->GetNumberOfIterations() << std::endl;
		std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
		std::cout << std::endl;
		std::cout << "No. elpased iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
		std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;

		try
		{
			writer4->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			std::cerr << "Error: " << error << std::endl;
		}
		// Display result of level set segmentation
		SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<OutputImageType2D>(thresholder->GetOutput()));
	}

	void runLevelSet3D(OutputImageType3D::Pointer itkImageData, const std::string outputFileName, const std::string outputDirectory)
	{
		const double sigma = 1.0;
	
		typedef itk::RescaleIntensityImageFilter<OutputImageType3D , InputImageType3D > CastFilterTypeOutputToInput;

		CastFilterTypeOutputToInput::Pointer caster = CastFilterTypeOutputToInput::New();
		caster->SetInput(itkImageData);
		caster->Update();
		
		//InputImageType3D::Pointer bilateralSmoothedImage = Denoising::bilateralFilterTemplate<InputImageType3D>(itkImageData, 2, 100);
		typedef  itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType3D, InputImageType3D > GradientFilterType;
		GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
		gradientMagnitude->SetSigma(sigma);		//gradientMagnitude->SetInput(readerMask->GetOutput());
		gradientMagnitude->SetInput(caster->GetOutput());

		typedef  itk::SigmoidImageFilter< InputImageType3D, InputImageType3D > SigmoidFilterType;
		SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
		sigmoid->SetOutputMinimum(0.0);
		sigmoid->SetOutputMaximum(1.0);
		sigmoid->SetAlpha(25);
		sigmoid->SetBeta(100);
		sigmoid->SetInput(gradientMagnitude->GetOutput());
		typedef itk::RescaleIntensityImageFilter< InputImageType3D, OutputImageType3D > CastFilterType;
				
		
		typedef  itk::FastMarchingImageFilter< InputImageType3D, InputImageType3D > FastMarchingFilterType;
		FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();

		typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType3D, InputImageType3D >  GeodesicActiveContourFilterType;
		GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
		geodesicActiveContour->SetPropagationScaling(3.0);
		geodesicActiveContour->SetCurvatureScaling(6.0);
		geodesicActiveContour->SetAdvectionScaling(10.0);
		geodesicActiveContour->SetMaximumRMSError(0.01);
		//geodesicActiveContour->SetReverseExpansionDirection(true);
		geodesicActiveContour->SetNumberOfIterations(3000);
		geodesicActiveContour->SetInput(fastMarching->GetOutput());
		geodesicActiveContour->SetFeatureImage(sigmoid->GetOutput());
	
		typedef itk::BinaryThresholdImageFilter< InputImageType3D, OutputImageType3D > ThresholdingFilterType;
		ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
		thresholder->SetLowerThreshold(-1000.0);
		thresholder->SetUpperThreshold(0.0);
		thresholder->SetOutsideValue(itk::NumericTraits< OutputPixelType2D >::min());
		thresholder->SetInsideValue(itk::NumericTraits< OutputPixelType2D >::max());
		thresholder->SetInput(geodesicActiveContour->GetOutput());
		
		typedef FastMarchingFilterType::NodeContainer  NodeContainer;
		typedef FastMarchingFilterType::NodeType       NodeType;

		InputImageType3D::IndexType  seedPosition;
		seedPosition[0] = 55;
		seedPosition[1] = 55;
		seedPosition[2] = 55;
		NodeContainer::Pointer seeds = NodeContainer::New();
		NodeType node;
		node.SetValue(-5.0);
		node.SetIndex(seedPosition);

		seeds->Initialize();
		seeds->InsertElement(0, node);

		fastMarching->SetTrialPoints(seeds);
		fastMarching->SetSpeedConstant(1.0);
		fastMarching->SetOutputSize(
			sigmoid->GetOutput()->GetBufferedRegion().GetSize());
		SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<OutputImageType3D>(thresholder->GetOutput()));
	}
}