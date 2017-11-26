#include "ActiveContour.h"

#include <itkVTKImageToImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>

ActiveContour::ActiveContour()
    : P( cv::Mat1d( 0, 0 ) )
    , m_elasticity( 0.01 )
    , m_stiffness( 0.4 )
    , m_speed( 0.07 )
    , m_smoothingSigma( 4 )
    , m_iterations( 10000 )
    , m_px( cv::Mat1d( 0, 0 ) )
    , m_py( cv::Mat1d( 0, 0 ) )
{

}

ActiveContour::~ActiveContour()
{
}

void ActiveContour::setImage( vtkImageData *image )
{
    typedef itk::VTKImageToImageFilter< ImageType2D > VTKTOITKFilterType;
    VTKTOITKFilterType::Pointer filter = VTKTOITKFilterType::New();
    filter->SetInput( image );

    itk::GradientMagnitudeImageFilter<ImageType2D, FloatImageType2D>::Pointer gradientMagnitudeFilter =
            itk::GradientMagnitudeImageFilter<ImageType2D, FloatImageType2D>::New();
    gradientMagnitudeFilter->SetInput( filter->GetOutput() );
    gradientMagnitudeFilter->Update();

    m_gradientImageFilter = itk::GradientRecursiveGaussianImageFilter<FloatImageType2D, OutputImageType >::New();
    m_gradientImageFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
    m_gradientImageFilter->SetSigma( m_smoothingSigma );
    m_gradientImageFilter->Update();

    m_gradientImage = m_gradientImageFilter->GetOutput();
}

void ActiveContour::setStartPoints( Contour points )
{
    m_px = cv::Mat1d( points.size(), 1 );
    m_py = cv::Mat1d( points.size(), 1 );
    for( size_t i = 0; i < points.size(); ++i )
    {
        m_px( i ) = points[i].x;
        m_py( i ) = points[i].y;
    }
}

void ActiveContour::setElasticity( double alpha )
{
    m_elasticity = alpha;
}

void ActiveContour::setStiffness( double beta )
{
    m_stiffness = beta;
}

void ActiveContour::setIterationSpeed( double gamma )
{
    m_speed = gamma;
}

void ActiveContour::setEdgeSoothingSigma( double sigma )
{
    m_smoothingSigma = sigma;
}

void ActiveContour::setMaxIterations( unsigned int iterations )
{
    m_iterations = iterations;
}

Contour ActiveContour::compute()
{
    createP();

    for( unsigned int i = 0; i < m_iterations; ++i )
    {
        iterationStep();
    }

    std::vector<cv::Point2d> points( m_px.rows );
    for( size_t i = 0; i < points.size(); ++i )
    {
        points[i].x = m_px( i );
        points[i].y = m_py( i );
    }
    return points;
}

void ActiveContour::init()
{
    createP();
}

Contour ActiveContour::step()
{
    iterationStep();

    Contour points( m_px.rows );
    for( size_t i = 0; i < points.size(); ++i )
    {
        points[i].x = m_px( i );
        points[i].y = m_py( i );
    }
    return points;
}

void ActiveContour::iterationStep()
{
    cv::Mat1d newX = P * ( m_px + m_speed * sampleImage( m_px, m_py, 0 ) );
    cv::Mat1d newY = P * ( m_py + m_speed * sampleImage( m_px, m_py, 1 ) );
    m_px = newX;
    m_py = newY;
}

void ActiveContour::createP()
{
    double a = m_speed*(2*m_elasticity+6*m_stiffness)+1;
    double b = m_speed*(-m_elasticity-4*m_stiffness);
    double c = m_speed*m_stiffness;

    int N = m_px.rows;
    P = cv::Mat1d( N, N, 0.0 );

    //fill diagonals
    P.diag(0).setTo( cv::Scalar( a ) );
    P.diag(1).setTo( cv::Scalar( b ) );
    P.diag(-1).setTo( cv::Scalar( b ) );
    P.diag(2).setTo( cv::Scalar( c ) );
    P.diag(-2).setTo( cv::Scalar( c ) );

    //Moreover
    P(0, N-1)=b;
    P(N-1, 0)=b;

    //Moreover
    P(0, N-2)=c;
    P(1, N-1)=c;
    P(N-2, 0)=c;
    P(N-1, 1)=c;

    if ( cv::determinant( P ) == 0.0 )
    {
        std::cerr << "ActiveContour: Singular P matrix. Determinant is 0." << std::endl;
    }

    //Compute the inverse of the matrix P
    P = P.inv();
}

cv::Mat1d ActiveContour::sampleImage(cv::Mat1d x, cv::Mat1d y, int gradientType)
{
    cv::Mat1d ans(x.rows,1);

    ImageType2D::IndexType index;
    for (int i=0; i<x.rows; i++)
    {
        index[0] = x(i);
        index[1] = y(i);
        ans(i) = m_gradientImage->GetPixel(index)[gradientType];
    }
    return ans;
}
