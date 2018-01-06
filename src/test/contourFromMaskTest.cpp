#include "fileHelpers/DICOMLoaderVTK.h"
#include "ContourFromMask.h"
#include "ColorTable.h"

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
#include <itkBinaryContourImageFilter.h>

#include <Helpers/TypeDefinitions.h>

vtkSmartPointer<vtkPolyData> createPolydataLine( std::vector<cv::Point2d> const & v )
{
    // Create a vtkPoints object and store the points in it
    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();
    for( int i = 0; i < v.size(); ++i )
    {
        points->InsertNextPoint( v[i].x, v[i].y, 0 );
    }
    points->InsertNextPoint( v[0].x, v[0].y, 0 );

    vtkSmartPointer<vtkPolyLine> polyLine =
            vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(v.size() + 1);
    for(unsigned int i = 0; i < v.size() + 1; i++)
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

    std::vector<Contour> contour = ContourFromMask::computeWithEdgeFilter( image, 0 );
    std::cout << "found " << contour.size() << " contours" << std::endl;
    for( size_t i = 0; i < contour.size(); ++i )
    {
        std::cout << "contour-" << i << " has size " << contour[i].size() << std::endl;
        contour[i] = ContourFromMask::simplify( contour[i], 1 );
        std::cout << "simplified contour-" << i << " to " << contour[i].size() << std::endl;
        contour[i] = ContourFromMask::resample( contour[i], 100 );
        std::cout << "resampled contour-" << i << " to " << contour[i].size() << std::endl;
    }

    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);


    itk::VTKImageToImageFilter<MaskType>::Pointer connector =
            itk::VTKImageToImageFilter<MaskType>::New();

    connector->SetInput( image );
    connector->Update();

    itk::BinaryContourImageFilter<MaskType, MaskType>::Pointer contourFilter =
            itk::BinaryContourImageFilter<MaskType, MaskType>::New();

    contourFilter->SetInput( connector->GetOutput() );
    contourFilter->SetBackgroundValue( 0 );
    contourFilter->SetForegroundValue( 1 );
    contourFilter->SetFullyConnected( true );
    contourFilter->Update();

    itk::ImageToVTKImageFilter<MaskType>::Pointer connectorITKVTK =
            itk::ImageToVTKImageFilter<MaskType>::New();

    connectorITKVTK->SetInput( contourFilter->GetOutput() );
    connectorITKVTK->Update();

    vtkImageData* maskContour = connectorITKVTK->GetOutput();

    vtkSmartPointer<vtkImageMapper> contourMapper =
            vtkSmartPointer<vtkImageMapper>::New();
    contourMapper->SetColorLevel( 0.5 );
    contourMapper->SetColorWindow( 1 );
    contourMapper->SetInputData( maskContour );

    vtkSmartPointer<vtkActor2D> contourActor =
            vtkSmartPointer<vtkActor2D>::New();
    contourActor->SetMapper(contourMapper);
    renderer->AddActor( contourActor );
    renderWindowInteractor->Start();
    renderer->RemoveActor( contourActor );


    vtkSmartPointer<vtkImageMapper> imageMapper =
            vtkSmartPointer<vtkImageMapper>::New();
    imageMapper->SetColorLevel( 0.5 );
    imageMapper->SetColorWindow( 1 );
    imageMapper->SetInputData( image );

    vtkSmartPointer<vtkActor2D> imageActor =
            vtkSmartPointer<vtkActor2D>::New();
    imageActor->SetMapper(imageMapper);

    renderer->AddActor(imageActor);

    for( size_t i = 0; i < contour.size(); ++i )
    {
        vtkSmartPointer<vtkPolyData> polyData =
                createPolydataLine( contour[i] );

        // Setup actor and mapper
        vtkSmartPointer<vtkPolyDataMapper2D> mapper =
                vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);

        vtkSmartPointer<vtkActor2D> actor =
                vtkSmartPointer<vtkActor2D>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor( colorTable[i % colorTableSize][0]  / 0.3,
                                        colorTable[i % colorTableSize][1]  / 0.3,
                                        colorTable[i % colorTableSize][2]  / 0.3 );
        actor->GetProperty()->SetLineWidth( 3 );
        actor->GetProperty()->SetPointSize( 5 );

        // Setup render window, renderer, and interactor
        renderer->AddActor(actor);
    }

    renderWindow->Render();

    renderWindowInteractor->Start();

    return 0;
}
