#include "fileHelpers/DICOMLoaderVTK.h"
#include "ContourFromMask.h"
#include "ActiveContour.h"
#include "ContourToMask.h"

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyLine.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkDataSetMapper.h>
#include <vtkImageMapper.h>

#include <opencv2/highgui.hpp>

#include <Denoising.h>
#include "Helpers/Validator.h"
#include "Helpers/Converter.h"

vtkSmartPointer<vtkPolyData> createPolydataLine( std::vector<cv::Point2d> const & v )
{
    // Create a vtkPoints object and store the points in it
    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();
    for( int i = 0; i < v.size(); ++i )
    {
        points->InsertNextPoint( v[i].x, v[i].y, 0 );
    }

    vtkSmartPointer<vtkPolyLine> polyLine =
            vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(v.size());
    for(unsigned int i = 0; i < v.size(); i++)
    {
        polyLine->GetPointIds()->SetId(i,i);
    }

    // Create a cell array to store the lines in and add the lines to it
    vtkSmartPointer<vtkCellArray> cells =
            vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell( polyLine );

    // Create a polydata to store everything in
    vtkSmartPointer<vtkPolyData> polyData =
            vtkSmartPointer<vtkPolyData>::New();

    // Add the points to the dataset
    polyData->SetPoints(points);

    // Add the lines to the dataset
    polyData->SetLines(cells);

    return polyData;
}

void updatePolydata( vtkSmartPointer<vtkPolyData> polyData, std::vector<cv::Point2d> const & v )
{
    vtkSmartPointer<vtkPoints> points = polyData->GetPoints();
    for( int i = 0; i < v.size(); ++i )
    {
        points->SetPoint( i, v[i].x, v[i].y, 0 );
    }
    polyData->SetPoints( points );
}

int main( int argc, char** argv )
{
    if( argc < 4 )
    {
        std::cerr << "usage: " << argv[0] << "[pathToDatasetImage] [pathToMaskImage] [pathToGroundTruth]" << std::endl;
        return -1;
    }

    vtkSmartPointer<vtkImageData> image =
            DICOMLoaderVTK::loadDICOM( std::string( argv[1] ) );

    vtkSmartPointer<vtkImageData> mask =
            DICOMLoaderVTK::loadDICOM( std::string( argv[2] ) );

    vtkSmartPointer<vtkImageData> groundTruth =
            DICOMLoaderVTK::loadDICOM( std::string( argv[3] ) );

    //std::cout << "start bilateral filtering" << std::endl;
    //image = Denoising::bilateralFilter( image, 2, 100 );
    //std::cout << "finshed bilateral filtering" << std::endl;

    vtkSmartPointer<vtkImageMapper> imageMapper =
            vtkSmartPointer<vtkImageMapper>::New();
    imageMapper->SetColorLevel( 185 );
    imageMapper->SetColorWindow( 100 );
    imageMapper->SetInputData( image );

    vtkSmartPointer<vtkActor2D> imageActor =
            vtkSmartPointer<vtkActor2D>::New();
    imageActor->SetMapper(imageMapper);

    // Setup render window, renderer, and interactor
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(imageActor);

    renderWindow->Render();

    //Snake parameters
    int nPoints = 500;
    double simplficationEps = 1;
    double alpha = 3;
    double beta = 1;
    double gamma = 0.1;
    double sigma = 7;
    double iterations = 1000;

    std::vector< std::vector<cv::Point2d> > contours = ContourFromMask::computeWithEdgeFilter( mask, 0 );
    std::vector< std::vector<cv::Point2d> > groundTruthContours = ContourFromMask::computeWithEdgeFilter( groundTruth, 0 );
    std::cout << "contours " << contours.size() << std::endl;
    std::vector<cv::Point2d> contour = contours[0];
    std::cout << "contour.size() = " << contour.size() << std::endl;
    if( contour.size() > nPoints )
    {
        contour = ContourFromMask::simplify( contour, simplficationEps );
        std::cout << "simplified contour.size() = " << contour.size() << std::endl;
        contour = ContourFromMask::resample( contour, nPoints );
        std::cout << "resampled contour.size() = " << contour.size() << std::endl;
    }

    vtkSmartPointer<vtkPolyData> polyData =
            createPolydataLine( contour );

    // Setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
            vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor2D> actor =
            vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
    actor->GetProperty()->SetLineWidth( 2 );

    renderer->AddActor(actor);


    ActiveContour activeContour;
    activeContour.setElasticity( alpha );
    activeContour.setStiffness( beta );
    activeContour.setIterationSpeed( gamma );
    activeContour.setEdgeSoothingSigma( sigma );
    activeContour.setMaxIterations( iterations );
    activeContour.setImage( image );

    activeContour.setStartPoints( contour );

    //activeContour.init();
    std::cerr << "start iteration" << std::endl;
    Contour finalContour = activeContour.compute();
    //for (int i = 0; i < iterations; i++)
    //{
    //    updatePolydata( polyData, activeContour.step() );
    //    polyData->Modified();
    //    mapper->Modified();
    //    renderer->Modified();
    //    if( i % 1 == 0 )
    //        renderWindow->Render();
    //
    //}
    //Contour finalContour =
    updatePolydata( polyData, finalContour );
    polyData->Modified();
    mapper->Modified();
    renderer->Modified();
    renderWindow->Render();
    std::cerr << "iteration finished" << std::endl;

    cv::imwrite( "mask.jpg", ContourToMask::compute( finalContour, image->GetDimensions() ) );

    std::cerr << "Average euclidean distance: "
              << Validator::averageEuclideanContourDistance( finalContour, groundTruthContours[0] )
              << std::endl;

    MaskType2D::Pointer finalMask = ContourToMask::computeITK( finalContour, image->GetDimensions() );
    std::cerr << "Dice Coeff: "
              << Validator::diceCoeff2DSlice( finalMask, Converter::ConvertVTKToITK<MaskType2D>( groundTruth ) )
              << std::endl;

    renderWindowInteractor->Start();

    return 0;
}
