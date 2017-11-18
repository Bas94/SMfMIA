#include "ActiveContour.h"

#include <itkVTKImageToImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>

typedef itk::Image<unsigned char, 2> ImageType;

ActiveContour::ActiveContour()
    : P( cv::Mat1d( 0, 0 ) )
    , m_alpha( 0.01 )
    , m_beta( 0.4 )
    , m_gamma( 0.07 )
    , m_sigma( 4 )
    , m_iterations( 10000 )
    , m_minDelta( 1e-8 )
    , m_px( cv::Mat1d( 0, 0 ) )
    , m_py( cv::Mat1d( 0, 0 ) )
{

}

void ActiveContour::setImage( vtkImageData *image )
{
    typedef itk::VTKImageToImageFilter< ImageType > VTKTOITKFilterType;
    VTKTOITKFilterType::Pointer filter = VTKTOITKFilterType::New();
    filter->SetInput( image );

    itk::GradientMagnitudeImageFilter<ImageType, FloatImageType>::Pointer gradientMagnitudeFilter =
            itk::GradientMagnitudeImageFilter<ImageType, FloatImageType>::New();
    gradientMagnitudeFilter->SetInput( filter->GetOutput() );
    gradientMagnitudeFilter->Update();

    m_gradientImageFilter = itk::GradientRecursiveGaussianImageFilter<FloatImageType, OutputImageType >::New();
    m_gradientImageFilter->SetInput( gradientMagnitudeFilter->GetOutput() );
    m_gradientImageFilter->SetSigma( m_sigma );
    m_gradientImageFilter->Update();

    m_gradientImage = m_gradientImageFilter->GetOutput();
}

void ActiveContour::setStartPoints( std::vector<cv::Point2d> points )
{
    m_px = cv::Mat1d( points.size(), 1 );
    m_py = cv::Mat1d( points.size(), 1 );
    for( size_t i = 0; i < points.size(); ++i )
    {
        m_px( i ) = points[i].x;
        m_py( i ) = points[i].y;
    }
}

void ActiveContour::setAlpha( double alpha )
{
    m_alpha = alpha;
}

void ActiveContour::setBeta( double beta )
{
    m_beta = beta;
}

void ActiveContour::setGamma( double gamma )
{
    m_gamma = gamma;
}

void ActiveContour::setSigma( double sigma )
{
    m_sigma = sigma;
}

void ActiveContour::setMaxIterations( unsigned int iterations )
{
    m_iterations = iterations;
}

void ActiveContour::setMinChangeDelta( double minDelta )
{
    m_minDelta = minDelta;
}

std::vector<cv::Point2d> ActiveContour::compute()
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

std::vector<cv::Point2d> ActiveContour::step()
{
    iterationStep();

    std::vector<cv::Point2d> points( m_px.rows );
    for( size_t i = 0; i < points.size(); ++i )
    {
        points[i].x = m_px( i );
        points[i].y = m_py( i );
    }
    return points;
}

void ActiveContour::iterationStep()
{
    cv::Mat1d newX = P * ( m_px + m_gamma * sampleImage( m_px, m_py, 0 ) );
    cv::Mat1d newY = P * ( m_py + m_gamma * sampleImage( m_px, m_py, 1 ) );
    m_px = newX;
    m_py = newY;
}

void ActiveContour::createP()
{
    double a = m_gamma*(2*m_alpha+6*m_beta)+1;
    double b = m_gamma*(-m_alpha-4*m_beta);
    double c = m_gamma*m_beta;

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

cv::Mat1d ActiveContour::sampleImage(cv::Mat1d x, cv::Mat1d y, int position)
{
    cv::Mat1d ans(x.rows,1);

    ImageType::IndexType index;
    for (int i=0; i<x.rows; i++)
    {
        index[0] = x(i);
        index[1] = y(i);
        ans(i) = m_gradientImage->GetPixel(index)[position];
    }
    return ans;
}
