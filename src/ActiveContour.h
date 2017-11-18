#pragma once
#ifndef ACTIVE_CONTOUR_H
#define ACTIVE_CONTOUR_H

#include <vector>
#include <opencv2/core.hpp>
#include <itkImage.h>
#include <itkGradientRecursiveGaussianImageFilter.h>

class vtkImageData;

/*!
 * \brief The ActiveContour class
 */

class ActiveContour
{
public:
    ActiveContour();

    void setImage( vtkImageData* image );
    void setStartPoints( std::vector<cv::Point2d> points );

    void setElasticity( double alpha );
    void setStiffness( double beta );
    void setIterationSpeed( double gamma );
    void setEdgeSoothingSigma( double sigma );

    void setMaxIterations( unsigned int iterations );
    void setMinChangeDelta( double minDelta );

    std::vector<cv::Point2d> compute();

    void init();
    std::vector<cv::Point2d> step();

protected:

    void iterationStep();
    void createP();
    cv::Mat1d sampleImage( cv::Mat1d x, cv::Mat1d y, int position );

    typedef itk::Image<float, 2> FloatImageType;
    typedef itk::Image<itk::CovariantVector<float, 2>, 2> OutputImageType;

    itk::GradientRecursiveGaussianImageFilter<FloatImageType, OutputImageType >::Pointer m_gradientImageFilter;
    OutputImageType::Pointer m_gradientImage;

    cv::Mat1d P;

    double m_elasticity;
    double m_stiffness;
    double m_speed;
    double m_smoothingSigma;
    double m_minDelta;

    unsigned int m_iterations;

    cv::Mat1d m_px;
    cv::Mat1d m_py;
};

#endif // ACTIVE_CONTOUR_H
