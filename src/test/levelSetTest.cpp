#include <vtkRenderer.h>

#include "SMfMIAImageViewer.h"
#include "fileHelpers/DICOMLoaderVTK.h"
#include "LevelSetSeg.h"
#include "ColorTable.h"
#include "SMfMIAInteractorStyle.h"

void displayImages(vtkSmartPointer<vtkImageData> imageData,
	std::vector< vtkSmartPointer<vtkImageData> > imageMasks)
{
	// Visualize
	vtkSmartPointer<SMfMIAImageViewer> imageViewer =
		vtkSmartPointer<SMfMIAImageViewer>::New();
	for (size_t i = 0; i < imageMasks.size(); ++i)
	{
		imageViewer->AddMask(imageMasks[i],
			colorTable[i % colorTableSize][0],
			colorTable[i % colorTableSize][1],
			colorTable[i % colorTableSize][2],
			0.2);
	}
	imageViewer->SetInputData(imageData);

	// Creats an interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	// Usage of own InteractionStyle
	vtkSmartPointer<SMfMIAInteractorStyle> myInteractorStyle =
		vtkSmartPointer<SMfMIAInteractorStyle>::New();
	myInteractorStyle->SetImageViewer(imageViewer);

	imageViewer->SetupInteractor(renderWindowInteractor);

	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

	imageViewer->SetSize(800, 600);
	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();
	imageViewer->SetSliceOrientationToXY();

	renderWindowInteractor->Start();
}



int main(int argc, char** argv)
{
	std::string fileDirectoryImage = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\10_data";
	std::string fileDirectoryMask = "C:\\DatenE\\02WiSe1718\\03SMMIA\\Projekt\\data\\p01\\1_int\\11_seg";

	vtkSmartPointer<vtkImageData> imageData;
	vtkSmartPointer<vtkImageData> imageMask;

	const double propagationScaling = 5;
	const double numberOfIterations = 10;

	imageData = DICOMLoaderVTK::loadDICOMSeries(fileDirectoryImage);
	imageMask = DICOMLoaderVTK::loadDICOMSeries(fileDirectoryMask);

	vtkSmartPointer<vtkImageData> levelSetSegImage = LevelSetSeg::levelSetSegmentation(imageData, imageMask, propagationScaling, numberOfIterations);
	std::vector< vtkSmartPointer<vtkImageData> > imageMasks;
	imageMasks.push_back(imageMask);

	displayImages(levelSetSegImage, imageMasks);

	return 0;
}