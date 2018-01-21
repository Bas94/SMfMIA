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
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkThresholdImageFilter.h"

namespace LevelSet
{
	OutputImageType2D::Pointer runLevelSet2D(const std::string inputFileName, LevelSet::ReaderType2D::Pointer readerMask, const std::string outputFileName, const std::string outputDirectory, const double sigma, const int alpha, const int beta, const double propagationScaling, const double curvaturScaling, const double advectionScaling, const double numberOfIterations)
	{	

		ReaderType2D::Pointer reader = ReaderType2D::New();
		reader->SetFileName(inputFileName);
		reader->Update();


		InputImageType2D::Pointer smoothedImageData = Denoising::bilateralFilterTemplate<InputImageType2D>(reader->GetOutput(), 2, 100);
		typedef  itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType2D, InputImageType2D > GradientFilterType;
		GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
		gradientMagnitude->SetSigma(sigma);		
		gradientMagnitude->SetInput(smoothedImageData);

		//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType2D>(gradientMagnitude->GetOutput()));

		typedef  itk::SigmoidImageFilter< InputImageType2D, InputImageType2D > SigmoidFilterType2D;
		SigmoidFilterType2D::Pointer sigmoid = SigmoidFilterType2D::New();
		sigmoid->SetOutputMinimum(0.0);
		sigmoid->SetOutputMaximum(1.0);
		sigmoid->SetAlpha(alpha);
		sigmoid->SetBeta(beta);
		sigmoid->SetInput(gradientMagnitude->GetOutput());
		//  The FastMarchingImageFilter requires the user to provide a seed
		//  point from which the level set will be generated. The user can actually
		//  pass not only one seed point but a set of them. Note the the
		//  FastMarchingImageFilter is used here only as a helper in the
		//  determination of an initial level set. We could have used the
		//  \doxygen{DanielssonDistanceMapImageFilter} in the same way.


		//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType2D>(sigmoid->GetOutput()));
		typedef  itk::FastMarchingImageFilter< InputImageType2D, InputImageType2D > FastMarchingFilterType;
		FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();

		typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType2D, InputImageType2D >  GeodesicActiveContourFilterType;
		GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
		geodesicActiveContour->SetPropagationScaling(propagationScaling);
		geodesicActiveContour->SetCurvatureScaling(curvaturScaling);
		geodesicActiveContour->SetAdvectionScaling(advectionScaling);
		geodesicActiveContour->SetMaximumRMSError(0.01);
		geodesicActiveContour->SetNumberOfIterations(numberOfIterations);
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
		node.SetValue(-(mean_d/4));//-5
		node.SetIndex(seedPosition);

		std::cout << "Seedpoint x: " << seedPoint[0] << " Seedpoint y: " << seedPoint[1] <<std::endl;
		std::cout << "Durchschnittliche Distanz: " << mean_d/2 << std::endl;
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


		caster1->SetInput(reader->GetOutput());
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


		fastMarching->SetOutputSize(
			reader->GetOutput()->GetBufferedRegion().GetSize());

	

		std::cout << std::endl;
		std::cout << "Max. no. iterations: " << geodesicActiveContour->GetNumberOfIterations() << std::endl;
		std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
		std::cout << std::endl;
		std::cout << "No. elpased iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
		std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;


		thresholder->Update();
		OutputImageType2D::Pointer test = thresholder->GetOutput();
		
		return test;
	}

	void runLevelSet3D(OutputImageType3D::Pointer itkImageData, const std::string outputFileName, const std::string outputDirectory)
	{
		const double sigma = 1.0;
		typedef itk::RescaleIntensityImageFilter<OutputImageType3D , InputImageType3D > CastFilterTypeOutputToInput3D;

		CastFilterTypeOutputToInput3D::Pointer caster = CastFilterTypeOutputToInput3D::New();
		caster->SetInput(itkImageData);
		caster->Update();
		SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType3D>(caster->GetOutput()));
		InputImageType3D::Pointer bilateralSmoothedImage = Denoising::bilateralFilterTemplate<InputImageType3D>(caster->GetOutput(), 2, 100);
		typedef  itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType3D, InputImageType3D > GradientFilterType;
		GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
		gradientMagnitude->SetSigma(sigma);		//gradientMagnitude->SetInput(readerMask->GetOutput());
		gradientMagnitude->SetInput(caster->GetOutput());
		
		typedef itk::SobelEdgeDetectionImageFilter< InputImageType3D, InputImageType3D >
			FilterType;
		FilterType::Pointer filter = FilterType::New();
		filter->SetInput(bilateralSmoothedImage);
		filter->Update();
		
	SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType3D>(filter->GetOutput()));
		
		//std::cout << "caster image dimension" << gradientMagnitude->GetOutput()<< std::endl;

		typedef  itk::SigmoidImageFilter< InputImageType3D, InputImageType3D > SigmoidFilterType;
		SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
		sigmoid->SetOutputMinimum(0.0);
		sigmoid->SetOutputMaximum(1.0);
		sigmoid->SetAlpha(25);
		sigmoid->SetBeta(300);
		sigmoid->SetInput(bilateralSmoothedImage);

		
		typedef  itk::FastMarchingImageFilter< InputImageType3D, InputImageType3D > FastMarchingFilterType;
		FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();

		typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType3D, InputImageType3D >  GeodesicActiveContourFilterType;
		GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
		geodesicActiveContour->SetPropagationScaling(2.0);
		geodesicActiveContour->SetCurvatureScaling(6.0);
		geodesicActiveContour->SetAdvectionScaling(8.0);
		geodesicActiveContour->SetMaximumRMSError(0.0001);
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
		seedPosition[0] = 60;
		seedPosition[1] = 73;
		seedPosition[2] = 2;
		NodeContainer::Pointer seeds = NodeContainer::New();
		NodeType node;
		node.SetValue(-5.0);
		node.SetIndex(seedPosition);

		seeds->Initialize();
		seeds->InsertElement(0, node);

		fastMarching->SetTrialPoints(seeds);
		fastMarching->SetSpeedConstant(1.0);
		sigmoid->Update();
		fastMarching->SetOutputSize(sigmoid->GetOutput()->GetBufferedRegion().GetSize());
		fastMarching->SetOutputSpacing(sigmoid->GetOutput()->GetSpacing());
		
		fastMarching->Update();
		std::cout << "fastMarching Output [0] " << fastMarching->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << "fastMarching Output [1] " << fastMarching->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << "fastMarching Output [2] " << fastMarching->GetOutput()->GetLargestPossibleRegion().GetSize()[2] << std::endl;
		std::cout << "sigmoid Output [0] " << sigmoid->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << "sigmoid Output [1] " << sigmoid->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << "sigmoid Output [2] " << sigmoid->GetOutput()->GetLargestPossibleRegion().GetSize()[2] << std::endl;
		std::cout << "No. elpased iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
		std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;

		SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<OutputImageType3D>(thresholder->GetOutput()));

	}
}