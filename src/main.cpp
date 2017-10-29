#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include <string.h>

#include "myVTKInteractorStyle.h"
#include "fileHelpers/DICOMLoaderVTK.h"

#include "fileHelpers/FileDialog.h"

// defines which handle read and display of a single DICOM Image or a DICOM Series
#define ONE_DICOM  false
#define SERIES_DICOM  true

// Function that display and enable to interact with DICOMs
void displayImages(vtkSmartPointer<vtkImageData> imageData)
{
	// Visualize
	vtkSmartPointer<vtkImageViewer2> imageViewer =
		vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetInputData(imageData);

	// Creats an interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	// Usage of own InteractionStyle
	vtkSmartPointer<myVTKInteractorStyle> myInteractorStyle =
		vtkSmartPointer<myVTKInteractorStyle>::New();
	myInteractorStyle->SetImageViewer(imageViewer);

	imageViewer->SetupInteractor(renderWindowInteractor);

	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

	imageViewer->SetSize(800, 600);
	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();
	renderWindowInteractor->Start();
}

int main(int argc, char** argv)
{	
	if (ONE_DICOM)
	{
        std::string inputFilename;
        if( !FileDialog::openFile( "C:\\develop", "*.dcm", inputFilename ) )
        {
            std::cerr << "No file was selected" << std::endl;
            return -1;
        }

		vtkSmartPointer<vtkImageData> imageData = DICOMLoaderVTK::loadDICOM(inputFilename);
		displayImages(imageData);
	}

	if (SERIES_DICOM)
	{
        std::string directory;
        if( !FileDialog::openFolder( "C:\\develop", directory ) )
        {
            std::cerr << "No folder was selected" << std::endl;
            return -1;
        }
		
		vtkSmartPointer<vtkImageData> imageData = DICOMLoaderVTK::loadDICOMSeries(directory);
		displayImages(imageData);
	}

    return 0;
}
