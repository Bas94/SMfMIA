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
	const std::string outputDirectory = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\10_data\\output";
	const std::string inputFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\data_3\\export0001.dcm";
	const std::string inputDirectory = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\0_pre\\data_4";
	const std::string outputFileName = "SegOutput.dcm";
	const std::string inputMaskFileName = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\seg_3\\export0001.dcm";


	const double sigma = 1.0;
	const double propagationScaling = 2.0;
	const double numberOfIterations = 3000.0;

	//vtkSmartPointer<vtkImageData> imageData = DICOMLoaderVTK::loadDICOMSeries(inputDirectory);
	//vtkSmartPointer<vtkImageData> smoothedImageData = Denoising::bilateralFilter(imageData, 2, 50);
	//LevelSet::OutputImageType2D::Pointer itkImageData = Converter::ConvertVTKToITK<LevelSet::OutputImageType2D>(imageData);
	//SMfMIAImageViewer::Show(imageData);


	LevelSet::runLevelSet2D(inputFileName, inputMaskFileName, outputFileName, outputDirectory);
	//LevelSet::runLevelSet3D(itkImageData, outputFileName, outputDirectory);
	return EXIT_SUCCESS;
}
