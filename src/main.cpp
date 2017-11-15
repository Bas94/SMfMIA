#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include <string.h>

#include "SMfMIAInteractorStyle.h"
#include "SMfMIAImageViewer.h"
#include "ColorTable.h"
#include "fileHelpers/DICOMLoaderVTK.h"
#include "fileHelpers/FileDialog.h"
#include "Denoising.h"

/*!
 * \brief The ProgramOptions struct holds all the data
 * given via command line or with file dialogs.
 * It holds all the filenames which are needed to load
 * the dataset and all masks which will be shown.
 */
struct ProgramOptions
{
    // directory to the dataset
    std::string datasetDirectory;
    // array of directory paths to mask data
    std::vector<std::string> maskDirectories;
};

/*!
 * \brief parseInput
 * \param argc and \p argv are the input variable of main function
 * \param options are the resulting parsed options for this program
 * \return returns true if parsing was successfull
 */
bool parseInput( int argc, char** argv, ProgramOptions & options )
{
    // clear all option fields
    options.datasetDirectory.clear();
    options.maskDirectories.clear();

    // check if we have some inputs via command line interface
    if( argc >= 2 )
    {
        options.datasetDirectory = std::string( argv[1] );
        for( int i = 2; i < argc; ++i )
        {
            options.maskDirectories.push_back( std::string( argv[i] ) );
        }
    }
    else
    {
        // get dataset directory via directory dialog
        if( !FileDialog::openFolder( "C:\\develop", options.datasetDirectory ) )
        {
            std::cerr << "No folder was selected" << std::endl;
            return false;
        }

        // ask for mask directories until user aborts
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
                options.maskDirectories.push_back( directoryMask );
            }
        }
    }
    return true;
}

/*!
 * \brief loadData takes directory paths and
 *        loads the image data behind them
 * \param options is the ProgramOptions object which holds
 *        all directories needed to load the data
 * \param imageData holds the resulting image data of the
 *        main dataset
 * \param imageMasks is an array of datasets where every
 *        dataset holds (hopefully binary) a mask image
 */
void loadData( ProgramOptions const & options,
               vtkSmartPointer<vtkImageData>& imageData,
               std::vector< vtkSmartPointer<vtkImageData> >& imageMasks )
{
    std::cout << "open dataset: " << options.datasetDirectory << std::endl;
    // load the dataset
    imageData = DICOMLoaderVTK::loadDICOMSeries( options.datasetDirectory );

    imageMasks.clear();
    // load mask data for every given path
    for( size_t i = 0; i < options.maskDirectories.size(); ++i )
    {
        std::cout << "open mask:    " << options.maskDirectories[i] << std::endl;
        vtkSmartPointer<vtkImageData> imageMask =
                DICOMLoaderVTK::loadDICOMSeries( options.maskDirectories[i] );
        imageMasks.push_back( imageMask );
    }
}

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




int main( int argc, char** argv )
{
    ProgramOptions options;

    // parse input and get all directories
    if( !parseInput( argc, argv, options ) )
    {
        return -1;
    }

    // load the whole data from the given directories
    vtkSmartPointer<vtkImageData> imageData;
    std::vector< vtkSmartPointer<vtkImageData> > imageMasks;
    loadData( options, imageData, imageMasks );
	
	// denoising image
	vtkSmartPointer<vtkImageData> smoothedImageData = Denoising::bilateralFilter(imageData,0.5,4);
   
	// display everything
    displayImages(smoothedImageData, imageMasks );
	displayImages(imageData, imageMasks);

    return 0;
}
