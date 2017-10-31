#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include <string.h>

#include "myVTKInteractorStyle.h"
#include "SMfMIAImageViewer.h"
#include "fileHelpers/DICOMLoaderVTK.h"

#include "fileHelpers/FileDialog.h"

// defines which handle read and display of a single DICOM Image or a DICOM Series
#define ONE_DICOM  false
#define SERIES_DICOM  true

// Function that display and enable to interact with DICOMs
void displayImages(vtkSmartPointer<vtkImageData> imageData,
                   vtkSmartPointer<vtkImageData> imageMask )
{
    // Visualize
    vtkSmartPointer<SMfMIAImageViewer> imageViewer =
        vtkSmartPointer<SMfMIAImageViewer>::New();
    imageViewer->SetMask( imageMask );
    imageViewer->SetInputData( imageData );

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
    imageViewer->SetSliceOrientationToXY();

    renderWindowInteractor->Start();
}

int main(int argc, char** argv)
{
    std::string directoryData( "" );
    std::string directoryMask( "" );

    // get dataset directory via directory dialog
    if( !FileDialog::openFolder( "C:\\develop", directoryData ) )
    {
        std::cerr << "No folder was selected" << std::endl;
        return -1;
    }

    // get mask directory via directory dialog
    if( !FileDialog::openFolder( "C:\\develop", directoryMask ) )
    {
        std::cerr << "No folder was selected" << std::endl;
        return -1;
    }

    // load the dataset
    vtkSmartPointer<vtkImageData> imageData =
            DICOMLoaderVTK::loadDICOMSeries( directoryData );
    // load mask data
    vtkSmartPointer<vtkImageData> imageMask =
            DICOMLoaderVTK::loadDICOMSeries( directoryMask );

    displayImages( imageData, imageMask );

    return 0;
}
