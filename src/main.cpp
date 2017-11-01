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

// color table with predefined colors
static int colorTableSize = 9;
static double colorTable[9][3] = {
    { 0.3, 0.0, 0.0 }, // red
    { 0.0, 0.3, 0.0 }, // green
    { 0.0, 0.0, 0.3 }, // blue
    { 0.3, 0.3, 0.0 }, // yellow
    { 0.0, 0.3, 0.3 }, // cyan
    { 0.3, 0.0, 0.3 }, // violett
    { 0.3, 0.1, 0.1 }, // brighter red
    { 0.1, 0.3, 0.1 }, // brighter green
    { 0.1, 0.1, 0.3 }  // brighter blue
};

// Function that display and enable to interact with DICOMs
void displayImages( vtkSmartPointer<vtkImageData> imageData,
                    std::vector< vtkSmartPointer<vtkImageData> > imageMasks )
{
    // Visualize
    vtkSmartPointer<SMfMIAImageViewer> imageViewer =
        vtkSmartPointer<SMfMIAImageViewer>::New();
    for( size_t i = 0; i < imageMasks.size(); ++i )
    {
        imageViewer->AddMask( imageMasks[i],
                              colorTable[i % colorTableSize][0],
                              colorTable[i % colorTableSize][1],
                              colorTable[i % colorTableSize][2],
                              0.2 );
    }
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

int main( int argc, char** argv )
{
    std::string directoryData( "" );
    std::vector<std::string> directoryMasks;

    if( argc >= 2 )
    {
        directoryData = std::string( argv[1] );
        for( int i = 2; i < argc; ++i )
        {
            directoryMasks.push_back( std::string( argv[i] ) );
        }
    }
    else
    {
        // get dataset directory via directory dialog
        if( !FileDialog::openFolder( "C:\\develop", directoryData ) )
        {
            std::cerr << "No folder was selected" << std::endl;
            return -1;
        }

        bool loadAnotherMask = true;
        while( loadAnotherMask )
        {
            std::string directoryMask;
            // get mask directory via directory dialog
            if( !FileDialog::openFolder( "C:\\develop", directoryMask ) )
            {
                loadAnotherMask = false;
            }
            else
            {
                directoryMasks.push_back( directoryMask );
            }
        }
    }

    std::cout << "open dataset: " << directoryData << std::endl;
    // load the dataset
    vtkSmartPointer<vtkImageData> imageData =
            DICOMLoaderVTK::loadDICOMSeries( directoryData );

    // load mask data
    std::vector< vtkSmartPointer<vtkImageData> > imageMasks;
    for( size_t i = 0; i < directoryMasks.size(); ++i )
    {
        std::cout << "open mask:    " << directoryMasks[i] << std::endl;
        vtkSmartPointer<vtkImageData> imageMask =
                DICOMLoaderVTK::loadDICOMSeries( directoryMasks[i] );
        imageMasks.push_back( imageMask );
    }

    // display everything
    displayImages( imageData, imageMasks );

    return 0;
}
