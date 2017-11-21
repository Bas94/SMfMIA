#include "fileHelpers/DICOMLoaderVTK.h"
#include "ContourFromMask.h"

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


int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        std::cerr << "usage: " << argv[0] << " [pathToMask]" << std::endl;
        return -1;
    }

    vtkSmartPointer<vtkImageData> image =
            DICOMLoaderVTK::loadDICOM( std::string( argv[1] ) );

    std::vector<cv::Point2d> contour = ContourFromMask::compute( image, 0 );

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
    actor->GetProperty()->SetLineWidth( 1 );

    vtkSmartPointer<vtkImageMapper> imageMapper =
            vtkSmartPointer<vtkImageMapper>::New();
    imageMapper->SetColorLevel( 0.5 );
    imageMapper->SetColorWindow( 1 );
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
    renderer->AddActor(actor);

    renderWindow->Render();

    renderWindowInteractor->Start();

    return 0;
}
