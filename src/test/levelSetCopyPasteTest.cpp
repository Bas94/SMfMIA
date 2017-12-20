#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include "Helpers/Converter.h"
#include "Denoising.h"
#include "SMfMIAImageViewer.h"
#include "LevelSetSeg.h"
int main(int argc, char* argv[])
{
	/*if (argc != 11)
	{
		std::cerr << "Usage: " << argv[0];
		std::cerr << " <InputFileName>  <OutputFileName>";
		std::cerr << " <seedX> <seedY> <InitialDistance>";
		std::cerr << " <Sigma> <SigmoidAlpha> <SigmoidBeta>";
		std::cerr << " <PropagationScaling> <NumberOfIterations>" << std::endl;
		return EXIT_FAILURE;
	}*/

	const char * inputFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\10_data\\export0004.dcm";
	const char * outputFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\10_data\\export0004Output.dcm";
	const char * inputMaskFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\01_seg\\export0004.dcm";
	const int seedPosX = 67;
	const int seedPosY = 59;

	const double initialDistance = 5.0;
	const double sigma = 1.0;
	const double alpha = -0.5;
	const double beta = 3.0;
	const double propagationScaling = 2.0;
	const double numberOfIterations = 3000.0;
	const double seedValue = -initialDistance;

	const unsigned int                Dimension = 2;

	typedef float                                    InputPixelType;
	typedef itk::Image< InputPixelType, Dimension >  InputImageType;
	typedef unsigned char                            OutputPixelType;
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

	typedef  itk::ImageFileReader< InputImageType >  ReaderType;
	typedef  itk::ImageFileWriter< OutputImageType > WriterType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(inputFileName);
	ReaderType::Pointer readerMask = ReaderType::New();
	readerMask->SetFileName(inputMaskFileName);
	readerMask->Update();

	typedef  itk::CurvatureAnisotropicDiffusionImageFilter< InputImageType, InputImageType > SmoothingFilterType;
	SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
	smoothing->SetTimeStep(0.125);
	smoothing->SetNumberOfIterations(5);
	smoothing->SetConductanceParameter(9.0);
	smoothing->SetInput(reader->GetOutput());

	typedef  itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType, InputImageType > GradientFilterType;
	GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
	gradientMagnitude->SetSigma(sigma);
	//gradientMagnitude->SetInput(readerMask->GetOutput());
	gradientMagnitude->SetInput(smoothing->GetOutput());

	InputImageType::Pointer bilateralSmoothedImage = Denoising::bilateralFilter2D<InputImageType>(reader->GetOutput(), 2, 100);

	SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType>(bilateralSmoothedImage));

	typedef  itk::SigmoidImageFilter< InputImageType, InputImageType > SigmoidFilterType;
	SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
	sigmoid->SetOutputMinimum(0.0);
	sigmoid->SetOutputMaximum(1.0);
	sigmoid->SetAlpha(alpha);
	sigmoid->SetBeta(beta);
	//sigmoid->SetInput(gradientMagnitude->GetOutput());
	sigmoid->SetInput(bilateralSmoothedImage);

	//  The FastMarchingImageFilter requires the user to provide a seed
	//  point from which the level set will be generated. The user can actually
	//  pass not only one seed point but a set of them. Note the the
	//  FastMarchingImageFilter is used here only as a helper in the
	//  determination of an initial level set. We could have used the
	//  \doxygen{DanielssonDistanceMapImageFilter} in the same way.
	typedef  itk::FastMarchingImageFilter< InputImageType, InputImageType > FastMarchingFilterType;
	FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();

	typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType, InputImageType >  GeodesicActiveContourFilterType;
	GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
	geodesicActiveContour->SetPropagationScaling(propagationScaling);
	geodesicActiveContour->SetCurvatureScaling(1.0);
	geodesicActiveContour->SetAdvectionScaling(1.0);
	geodesicActiveContour->SetMaximumRMSError(0.02);
	//geodesicActiveContour->SetReverseExpansionDirection(true);
	geodesicActiveContour->SetNumberOfIterations(numberOfIterations);
	geodesicActiveContour->SetInput(fastMarching->GetOutput());
	geodesicActiveContour->SetFeatureImage(sigmoid->GetOutput());



	typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType > ThresholdingFilterType;
	ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
	thresholder->SetLowerThreshold(-1000.0);
	thresholder->SetUpperThreshold(0.0);
	thresholder->SetOutsideValue(itk::NumericTraits< OutputPixelType >::min());
	thresholder->SetInsideValue(itk::NumericTraits< OutputPixelType >::max());
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
	std::vector<int> seedPoints = LevelSetSeg::computeMeanValueOfMask<InputImageType>(readerMask->GetOutput());

	InputImageType::IndexType  seedPosition;
	seedPosition[0] = seedPoints[0];
	seedPosition[1] = seedPoints[1];

	NodeContainer::Pointer seeds = NodeContainer::New();
	NodeType node;
	node.SetValue(-30);
	node.SetIndex(seedPosition);

	seeds->Initialize();
	seeds->InsertElement(0, node);

	fastMarching->SetTrialPoints(seeds);
	fastMarching->SetSpeedConstant(1.0);

	typedef itk::RescaleIntensityImageFilter< InputImageType, OutputImageType > CastFilterType;

	CastFilterType::Pointer caster1 = CastFilterType::New();
	CastFilterType::Pointer caster2 = CastFilterType::New();
	CastFilterType::Pointer caster3 = CastFilterType::New();
	CastFilterType::Pointer caster4 = CastFilterType::New();

	WriterType::Pointer writer1 = WriterType::New();
	WriterType::Pointer writer2 = WriterType::New();
	WriterType::Pointer writer3 = WriterType::New();
	WriterType::Pointer writer4 = WriterType::New();

	//SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<InputImageType>(sigmoid->GetOutput()));

	caster1->SetInput(smoothing->GetOutput());
	writer1->SetInput(caster1->GetOutput());
	writer1->SetFileName("C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\00_data\\export0002GeodesicActiveContourImageFilterOutput1.dcm");
	caster1->SetOutputMinimum(itk::NumericTraits< OutputPixelType >::min());
	caster1->SetOutputMaximum(itk::NumericTraits< OutputPixelType >::max());
	writer1->Update();

	caster2->SetInput(gradientMagnitude->GetOutput());
	writer2->SetInput(caster2->GetOutput());
	writer2->SetFileName("C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\00_data\\export0002GeodesicActiveContourImageFilterOutput2.dcm");
	caster2->SetOutputMinimum(itk::NumericTraits< OutputPixelType >::min());
	caster2->SetOutputMaximum(itk::NumericTraits< OutputPixelType >::max());
	writer2->Update();

	caster3->SetInput(sigmoid->GetOutput());
	writer3->SetInput(caster3->GetOutput());
	writer3->SetFileName("C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\00_data\\export0002GeodesicActiveContourImageFilterOutput3.dcm");
	caster3->SetOutputMinimum(itk::NumericTraits< OutputPixelType >::min());
	caster3->SetOutputMaximum(itk::NumericTraits< OutputPixelType >::max());
	writer3->Update();

	caster4->SetInput(geodesicActiveContour->GetOutput());
	writer4->SetInput(caster4->GetOutput());
	writer4->SetFileName("C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\00_data\\export0002GeodesicActiveContourImageFilterOutput4.dcm");
	caster4->SetOutputMinimum(itk::NumericTraits< OutputPixelType >::min());
	caster4->SetOutputMaximum(itk::NumericTraits< OutputPixelType >::max());

	fastMarching->SetOutputSize(
		reader->GetOutput()->GetBufferedRegion().GetSize());

	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(outputFileName);
	writer->SetInput(thresholder->GetOutput());
	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
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
		return EXIT_FAILURE;
	}

	typedef itk::ImageFileWriter< InputImageType > InternalWriterType;

	InternalWriterType::Pointer mapWriter = InternalWriterType::New();
	mapWriter->SetInput(fastMarching->GetOutput());
	mapWriter->SetFileName("GeodesicActiveContourImageFilterOutput4.mha");
	try
	{
		mapWriter->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
	}

	InternalWriterType::Pointer speedWriter = InternalWriterType::New();
	speedWriter->SetInput(sigmoid->GetOutput());
	speedWriter->SetFileName("GeodesicActiveContourImageFilterOutput3.mha");
	try
	{
		speedWriter->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
	}

	InternalWriterType::Pointer gradientWriter = InternalWriterType::New();
	gradientWriter->SetInput(gradientMagnitude->GetOutput());
	gradientWriter->SetFileName("GeodesicActiveContourImageFilterOutput2.mha");
	try
	{
		gradientWriter->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}