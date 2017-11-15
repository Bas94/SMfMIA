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

#include <opencv2/core.hpp>
#include <iostream>

namespace
{
typedef itk::Image< unsigned char, 2 >    ImageType;
typedef itk::Image< float,  2 >           FloatImageType;
typedef ImageType::IndexType              IndexType;
typedef itk::CovariantVector< float, 2  > OutputPixelType;
typedef itk::Image< OutputPixelType, 2 >  OutputImageType;
typedef itk::GradientRecursiveGaussianImageFilter<
FloatImageType, OutputImageType>        FilterType;
typedef itk::GradientMagnitudeImageFilter<
ImageType, FloatImageType >             GradMagfilterType;
typedef itk::ImageFileReader< ImageType > ReaderType;
}

cv::Mat1d generateCircle( double cx, double cy, double rx, double ry, int n);
void createImage(ImageType::Pointer image,
                 int w, int h, double cx, double cy, double rx, double ry);
cv::Mat1d computeP(double alpha, double beta, double gamma, double N) throw (int);
cv::Mat1d sampleImage(cv::Mat1d x, cv::Mat1d y, OutputImageType::Pointer gradient, int position);

vtkSmartPointer<vtkPolyData> createPolydataLine( cv::Mat1d const & v )
{
    // Create a vtkPoints object and store the points in it
    vtkSmartPointer<vtkPoints> points =
            vtkSmartPointer<vtkPoints>::New();
    for( int i = 0; i < v.rows; ++i )
    {
        points->InsertNextPoint( v(2*i+0), v(2*i+1), 0 );
    }

    vtkSmartPointer<vtkPolyLine> polyLine =
            vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(v.rows);
    for(unsigned int i = 0; i < v.rows; i++)
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

int main( int argc, char* argv[] )
{
    //Image dimensions
    int w = 300;
    int h = 300;
    ImageType::Pointer image;
    if (argc < 7)
    {
        std::cout << "Usage " << argv[0]
                  << " points alpha beta gamma sigma iterations [image]"
                  << std::endl;
        return EXIT_SUCCESS;;
    }
    else if (argc < 8)
    {
        //Synthesize the image
        image = ImageType::New();
        createImage(image, w, h, 150, 150, 50, 50);
    }
    else if (argc == 8)
    {
        //Open the image
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( argv[7] );
        try
        {
            reader->Update();
            image = reader->GetOutput();
            w = image->GetLargestPossibleRegion().GetSize()[0];
            h = image->GetLargestPossibleRegion().GetSize()[1];
        }
        catch( itk::ExceptionObject & err )
        {
            std::cerr << "Caught unexpected exception " << err;
            return EXIT_FAILURE;
        }
    }

    //Snake parameters
    double alpha = 0.001;
    double beta = 0.4;
    double gamma = 100;
    double iterations = 1;
    int nPoints = 20;
    double sigma;

    nPoints = atoi(argv[1]);
    alpha = atof(argv[2]);
    beta = atof(argv[3]);
    gamma = atof(argv[4]);
    sigma = atof(argv[5]);
    iterations = atoi(argv[6]);

    //Temporal variables
    cv::Mat1d P;
    cv::Mat1d v;
    double N;

    //Generate initial snake circle
    v = generateCircle(130, 130, 50, 50, nPoints);

    vtkSmartPointer<vtkPolyData> polyData =
            createPolydataLine( v );

    // Setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
            vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor2D> actor =
            vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);

    // Setup render window, renderer, and interactor
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);

    renderWindow->Render();


    //Computes P matrix.
    N = v.rows/2;
    try
    {
        P = computeP(alpha, beta, gamma, N);
    }
    catch (int n)
    {
        return EXIT_FAILURE;;
    }

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

    //Loop
    cv::Mat1d x(N,1);
    cv::Mat1d y(N,1);

    std::cout << "Initial snake" << std::endl;
    for (int i = 0; i < N; i++)
    {
        x(i) = v(2*i);
        y(i) = v(2*i+1);
        std::cout << "(" << x(i) << ", " << y(i) << ")" << std::endl;
    }

    for (int i = 0; i < iterations; i++)
    {

        x = (x+gamma*sampleImage(x, y, gradientFilter->GetOutput(), 0));
        x = (x.t() * P).t();
        y = (y+gamma*sampleImage(x, y, gradientFilter->GetOutput(), 1));
        y = ( y.t() * P ).t();
    }

    //Display the answer
    std::cout << "Final snake after " << iterations << " iterations" << std::endl;
    cv::Mat1d v2(2*N,1);
    for (int i=0; i<N; i++)
    {
        v2(2*i) = x(i);
        v2(2*i+1) = y(i);
        std::cout << "(" << x(i) << ", " << y(i) << ")" << std::endl;
    }

    return EXIT_SUCCESS;;
}

cv::Mat1d generateCircle( double cx, double cy, double rx, double ry, int n)
{
    cv::Mat1d v(2*(n+1),1);

    for (int i=0; i<n; i++)
    {
        v(2*i) = cx + rx*cos(2*M_PI*i/n);
        v(2*i+1) = cy + ry*sin(2*M_PI*i/n);
    }
    v(2*n)=v(0);
    v(2*n+1)=v(1);
    return v;
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
            if ( ((i-cx)*(i-cx)/(rx*rx) + (j-cy)*(j-cy)/(ry*ry) ) < 1)
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

cv::Mat1d computeP(double alpha, double beta, double gamma, double N) throw (int)
{

    double a = gamma*(2*alpha+6*beta)+1;
    double b = gamma*(-alpha-4*beta);
    double c = gamma*beta;

    cv::Mat1d P( N, N, 0.0 );

    //fill diagonal
    cv::Mat1d diag = P.diag();
    diag.setTo( cv::Scalar( a ) );

    //fill next two diagonals
    for (int i=0; i<(N-1); i++)
    {
        P(i+1,i) = b;
        P(i,i+1) = b;
    }
    //Moreover
    P(0, N-1)=b;
    P(N-1, 0)=b;

    //fill next two diagonals
    for (int i=0; i<(N-2); i++)
    {
        P(i+2,i) = c;
        P(i,i+2) = c;
    }
    //Moreover
    P(0, N-2)=c;
    P(1, N-1)=c;
    P(N-2, 0)=c;
    P(N-1, 1)=c;

    if ( cv::determinant( P ) == 0.0 )
    {
        std::cerr << "Singular matrix. Determinant is 0." << std::endl;
        throw 2;
    }

    //Compute the inverse of the matrix P
    cv::Mat1d Pinv;
    Pinv = P.inv();

    return Pinv.t();
}

cv::Mat1d sampleImage(cv::Mat1d x, cv::Mat1d y, OutputImageType::Pointer gradient, int position)
{
    int size;
    size = x.rows;
    cv::Mat1d ans(size,1);

    IndexType index;
    for (int i=0; i<size; i++)
    {
        index[0] = x(i);
        index[1] = y(i);
        ans(i) = gradient->GetPixel(index)[position];
    }
    return ans;
}
