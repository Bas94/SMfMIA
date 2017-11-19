#include "itkImage.h"
#include "itkRandomImageSource.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkImageFileReader.h"

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
#include <itkImageToVTKImageFilter.h>

#include <opencv2/core.hpp>
#include <iostream>

#include "ActiveContour.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif // M_PI

namespace
{
typedef itk::Image<unsigned char, 2> ImageType;
typedef itk::Image<float, 2>         FloatImageType;
typedef ImageType::IndexType         IndexType;
typedef itk::Image<itk::CovariantVector<float, 2>, 2>  OutputImageType;
typedef itk::GradientRecursiveGaussianImageFilter<
FloatImageType, OutputImageType>        FilterType;
typedef itk::GradientMagnitudeImageFilter<ImageType, FloatImageType> GradMagfilterType;
}

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

std::vector<cv::Point2d> generateCircle( double cx, double cy, double rx, double ry, int n)
{
    std::vector<cv::Point2d> points( n );

    for (int i=0; i<n; i++)
    {
        points[i].x = cx + rx*cos(2*M_PI*i/n);
        points[i].y = cy + ry*sin(2*M_PI*i/n);
    }
    return points;
}

void createImage(ImageType::Pointer image,
                 int w, int h, double cx, double cy, double rx, double ry)
{

    itk::Size<2> size;
    size[0] = w;
    size[1] = h;

    itk::RandomImageSource<ImageType>::Pointer randomImageSource = itk::RandomImageSource<ImageType>::New();
    randomImageSource->SetNumberOfThreads(1); // to produce non-random results
    randomImageSource->SetSize(size);
    randomImageSource->SetMin(200);
    randomImageSource->SetMax(255);
    randomImageSource->Update();

    image->SetRegions(randomImageSource->GetOutput()->GetLargestPossibleRegion());
    image->Allocate();

    IndexType index;

    //Draw oval.
    for (int i=0; i<w; i++)
    {
        for (int j=0; j<h; j++)
        {
            index[0] = i; index[1] = j;
            if( ((i-cx)*(i-cx)/(rx*rx) + (j-cy)*(j-cy)/(ry*ry) ) < 1 ||
                ((i-cx)*(i-cx)/(rx/2*rx/2) + (j-cy+ry/1.5)*(j-cy+ry/1.5)/(ry/1.5*ry/1.5) ) < 1)
            {
                image->SetPixel(index, randomImageSource->GetOutput()->GetPixel(index)-100);
            }
            else
            {
                image->SetPixel(index, randomImageSource->GetOutput()->GetPixel(index));
            }

        }
    }
}

int main( int argc, char* argv[] )
{
    //Image dimensions
    int w = 300;
    int h = 300;
    ImageType::Pointer image;
    //Synthesize the image
    image = ImageType::New();
    createImage(image, w, h, 150, 150, 55, 55);

    //Snake parameters
    int nPoints = 100;
    double alpha = 0.01;
    double beta = 0.2;
    double gamma = 0.7;
    double sigma = 10;
    double iterations = 2000;

    if( argc > 1 ) nPoints = atoi(argv[1]);
    if( argc > 2 ) alpha = atof(argv[2]);
    if( argc > 3 ) beta = atof(argv[3]);
    if( argc > 4 ) gamma = atof(argv[4]);
    if( argc > 5 ) sigma = atof(argv[5]);
    if( argc > 6 ) iterations = atoi(argv[6]);

    //Generate initial snake circle
    std::vector<cv::Point2d> v = generateCircle(130, 130, 50, 50, nPoints);

    //Computes the magnitude gradient
    GradMagfilterType::Pointer gradientMagnitudeFilter =
            GradMagfilterType::New();
    gradientMagnitudeFilter->SetInput( image );
    gradientMagnitudeFilter->Update();

    //Computes the gradient of the gradient magnitude
    FilterType::Pointer gradientFilter = FilterType::New();
    gradientFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
    gradientFilter->SetSigma( sigma );
    gradientFilter->Update();

    vtkSmartPointer<vtkPolyData> polyData =
            createPolydataLine( v );

    // Setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
            vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor2D> actor =
            vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
    actor->GetProperty()->SetLineWidth( 3 );

#if 0
    typedef itk::ImageToVTKImageFilter<FloatImageType>       ConnectorType;
    ConnectorType::Pointer connector = ConnectorType::New();

    connector->SetInput(gradientMagnitudeFilter->GetOutput());
    connector->Update();

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->DeepCopy(connector->GetOutput());
    vtkSmartPointer<vtkImageMapper> imageMapper =
        vtkSmartPointer<vtkImageMapper>::New();
    imageMapper->SetColorLevel(0);
    imageMapper->SetColorWindow( 100 );
      imageMapper->SetInputData( imageData );
#else
    typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;
    ConnectorType::Pointer connector = ConnectorType::New();

    connector->SetInput(image);
    connector->Update();

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->DeepCopy(connector->GetOutput());
    vtkSmartPointer<vtkImageMapper> imageMapper =
        vtkSmartPointer<vtkImageMapper>::New();
    imageMapper->SetColorLevel(225);
    imageMapper->SetColorWindow( 100 );
      imageMapper->SetInputData( imageData );
#endif

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

    ActiveContour activeContour;
    activeContour.setElasticity( alpha );
    activeContour.setStiffness( beta );
    activeContour.setIterationSpeed( gamma );
    activeContour.setEdgeSoothingSigma( sigma );
    activeContour.setMaxIterations( iterations );
    activeContour.setImage( imageData );

    activeContour.setStartPoints( v );

#if 1
    activeContour.init();
    std::cerr << "start iteration" << std::endl;
    for (int i = 0; i < iterations; i++)
    {
        updatePolydata( polyData, activeContour.step() );
        polyData->Modified();
        mapper->Modified();
        renderer->Modified();
        if( i % 10 == 0 )
            renderWindow->Render();

    }
    std::cerr << "iteration finished" << std::endl;
#else
    activeContour.compute();
    updatePolydata( polyData, activeContour.step() );
    polyData->Modified();
    mapper->Modified();
    renderer->Modified();
    renderWindow->Render();
#endif

    renderWindowInteractor->Start();

    return 0;
}
