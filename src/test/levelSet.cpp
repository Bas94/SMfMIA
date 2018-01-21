#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include "Helpers/Converter.h"
#include "Denoising.h"
#include "SMfMIAImageViewer.h"
#include "LevelSetSeg.h"
#include "LevelSetFunctions.h"
#include "fileHelpers/DICOMLoaderVTK.h"
#include "Helpers/Validator.h"
#include "fileHelpers/WriteDCM.h"




int main(int argc, char* argv[])
{
	int sigma = 1, alpha = 1, beta = 1;
	double propagationScaling = 1.0, curvaturScaling = 1.0, advectionScaling = 1.0, numberOfIterations = 1.0;
	int j = 0.0;
	std::string patient;
	if (argc == 10)
	{
		
		propagationScaling = std::atof(argv[1]);
		curvaturScaling = std::atof(argv[2]);
		advectionScaling = std::atof(argv[3]);
		numberOfIterations = std::atof(argv[4]);
		sigma = std::atof(argv[5]);
		alpha = std::atof(argv[6]);
		beta = std::atof(argv[7]);
		j = std::atof(argv[8]);
		patient = argv[9];
	}
	
	std::string outputDirectory = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\"  + patient +  "\\1_int\\output\\";
	std::string inputFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\" + patient + "\\1_int\\10_data\\";
	std::string inputMaskFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\" + patient +"\\1_int\\11_seg\\";

	std::string outputFileName = "SegOutput";
	
	double maxDice = 0.0;
	int numberOfSlice = 0;	
	LevelSet::OutputImageType2D::Pointer outputMask;
	
	std::string filename;
	if (j < 10)
	{
		filename = "export000";
		filename += std::to_string(j);
		filename += ".dcm";
	}
	else
	{
		filename = "export00";
		filename += std::to_string(j);
		filename += ".dcm";
	}
		
	// read in the preoperative mask
	LevelSet::ReaderType2D::Pointer readerMask = LevelSet::ReaderType2D::New();
	readerMask->SetFileName(inputMaskFileName + filename);
	readerMask->Update();

	// read in the intraoperative image
	LevelSet::ReaderType2D::Pointer reader = LevelSet::ReaderType2D::New();
	reader->SetFileName(inputFileName + filename);
	reader->Update();

	// calculates the mask with level set algorithm
	outputMask = LevelSet::runLevelSet2D(reader, readerMask, outputFileName + filename, outputDirectory, sigma, alpha, beta, propagationScaling, curvaturScaling, advectionScaling, numberOfIterations);

	typedef itk::RescaleIntensityImageFilter<LevelSet::OutputImageType2D, MaskType2D > CastFilterTypeOutputToInput;
	CastFilterTypeOutputToInput::Pointer caster = CastFilterTypeOutputToInput::New();
	caster->SetInput(outputMask);
	caster->Update();

	typedef itk::RescaleIntensityImageFilter<LevelSet::InputImageType2D, MaskType2D > CastFilterTypeOutputToInputGround;
	CastFilterTypeOutputToInputGround::Pointer caster2 = CastFilterTypeOutputToInputGround::New();
	caster2->SetInput(readerMask->GetOutput());
	caster2->Update();
	
	//calculates the dicecoefficient
	double dicecoeff = Validator::diceCoeff2DSlice(caster->GetOutput(), caster2->GetOutput());
	
	std::cout << "Dicecoeff: " << dicecoeff << std::endl;
	
	writeDCMSeries<LevelSet::OutputImageType2D>(outputMask, outputFileName + filename, outputDirectory);
	
	SMfMIAImageViewer::Show(Converter::ConvertITKToVTK<LevelSet::OutputImageType2D>(outputMask));

	//LevelSet::runLevelSet3D(itkImageData, outputFileName, outputDirectory);
	
	return EXIT_SUCCESS;
}
