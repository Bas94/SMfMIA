#include "fileHelpers/DICOMLoaderVTK.h"
#include "ContourFromMask.h"
#include "ActiveContour.h"
#include "ContourToMask.h"

#include <vtkExtractVOI.h>

#include <opencv2/highgui.hpp>
#include "SMfMIAImageViewer.h"
#include "SMfMIAInteractorStyle.h"
#include <vtkRenderer.h>

int main( int argc, char** argv )
{
    if( argc < 3 )
    {
        std::cerr << "usage: " << argv[0] << " [pathToDatasetImage] [pathToMaskImage]" << std::endl;
        return -1;
    }

    int nPoints = 500;
    double simplficationEps = 1;
    double elasticity = 1;
    double stiffness = 500;
    double speed = 0.2;
    double smoothingSigma = 2;
    double iterations = 2000;

    std::cerr << "load data" << std::endl;
    vtkSmartPointer<vtkImageData> image =
            DICOMLoaderVTK::loadDICOMSeries( std::string( argv[1] ) );

    vtkSmartPointer<vtkImageData> mask =
            DICOMLoaderVTK::loadDICOMSeries( std::string( argv[2] ) );

    int* dim = mask->GetDimensions();

    std::cerr << "find initial contours" << std::endl;
    std::vector< std::vector<Contour> > contoursPerSclice;
    for( int i = 0; i < mask->GetDimensions()[2]; ++i )
    {
        vtkSmartPointer<vtkExtractVOI> voiExtractor =
                vtkSmartPointer<vtkExtractVOI>::New();
        voiExtractor->SetInputData( mask );
        voiExtractor->SetVOI( 0, dim[0]-1, 0, dim[1]-1, i, i );
        voiExtractor->Update();
        voiExtractor->GetOutput()->SetExtent( 0, dim[0]-1, 0, dim[1]-1, 0, 0 );

        std::vector<Contour> contoursInSlice = ContourFromMask::computeWithEdgeFilter( voiExtractor->GetOutput(), 0 );
        for( size_t j = 0; j < contoursInSlice.size(); ++j )
        {
            // if number of points is small enough, don't simplify
            if( contoursInSlice[j].size() < nPoints )
                continue;

            contoursInSlice[j] = ContourFromMask::simplify( contoursInSlice[j], simplficationEps );
            contoursInSlice[j] = ContourFromMask::resample( contoursInSlice[j], nPoints );
        }
        if( contoursInSlice.size() > 3 )
            std::cerr << "contours found in slice bigger than 3: " << contoursInSlice.size() << " in slice " << i << std::endl;
        contoursPerSclice.push_back( contoursInSlice );
    }

    std::cerr << "initalizing contour algorithm" << std::endl;
    std::cerr << "mask dimension: " << dim[0] << " " << dim[1] << " " << dim[2] << std::endl;
    // initialize all algorithms
    std::vector<std::vector<ActiveContour> > activeContourAlgorithms( contoursPerSclice.size() );
    for( size_t i = 0; i < contoursPerSclice.size(); ++i )
    {
        vtkSmartPointer<vtkExtractVOI> voiExtractor =
                vtkSmartPointer<vtkExtractVOI>::New();
        voiExtractor->SetInputData( image );
        voiExtractor->SetVOI( 0, dim[0]-1, 0, dim[1]-1, i, i );
        voiExtractor->Update();
        voiExtractor->GetOutput()->SetExtent( 0, dim[0]-1, 0, dim[1]-1, 0, 0 );

        activeContourAlgorithms[i].resize( contoursPerSclice[i].size() );
        for( size_t j = 0; j < contoursPerSclice[i].size(); ++j )
        {
            ActiveContour& activeContour = activeContourAlgorithms[i][j];
            activeContour.setMaxIterations( iterations );
            activeContour.setElasticity( elasticity );
            activeContour.setStiffness( stiffness );
            activeContour.setIterationSpeed( speed );
            activeContour.setEdgeSoothingSigma( smoothingSigma );
            activeContour.setStartPoints( contoursPerSclice[i][j] );

            activeContour.setImage( voiExtractor->GetOutput() );
        }
    }

    std::cerr << "optimizing contours" << std::endl;
    // fit curves:
    std::vector< std::vector<Contour> > finalContoursPerSclice( contoursPerSclice.size() );
    for( size_t i = 0; i < activeContourAlgorithms.size(); ++i )
    {
        finalContoursPerSclice[i].resize( activeContourAlgorithms[i].size() );
        for( size_t j = 0; j < activeContourAlgorithms[i].size(); ++j )
        {
             finalContoursPerSclice[i][j] = activeContourAlgorithms[i][j].compute();
        }
    }

    std::cerr << "converting contours to masks and saving" << std::endl;
    std::vector<cv::Mat3b> masksSlicewise;
    // covert curves to masks
    for( size_t i = 0; i < finalContoursPerSclice.size(); ++i )
    {
        cv::Mat3b sliceMask( dim[1], dim[0], cv::Vec3b( 0, 0, 0 ) );
        for( size_t j = 0; j < finalContoursPerSclice[i].size(); ++j )
        {
            cv::Mat3b mask = ContourToMask::compute( finalContoursPerSclice[i][j], dim );
            cv::add( sliceMask, mask, sliceMask );
        }
        std::stringstream ss;
        ss << "mask" << std::setfill('0') << std::setw(2) << i << ".jpg";
        cv::imwrite( ss.str(), sliceMask );
        masksSlicewise.push_back( sliceMask );
    }
}
