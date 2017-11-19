#pragma once
#ifndef ACTIVE_CONTOUR_H
#define ACTIVE_CONTOUR_H

#include <vector>
#include <opencv2/core.hpp>
#include <itkImage.h>
#include <itkGradientRecursiveGaussianImageFilter.h>

class vtkImageData;

/*!
 * \brief The ActiveContour class implements active contours algorithm
 * which optimizes a given contour with repect to the edges within the
 * image.
 *
 * The ActiveContour algorith can be parametrized with influence on
 * elasticity, stiffness, iteration speed and smoothness of the used
 * edge image computed from the original image.
 */

class ActiveContour
{
public:
    /*!
     * \brief ActiveContour creates the object with common
     * parameter initialization.
     */
    ActiveContour();
    ~ActiveContour();

    /*!
     * \brief setImage sets the current image where to optimize on.
     * Set the edge soothing sigma before calling this to influence
     * the smoothness of the gradient image.
     * \see setEdgeSoothingSigma()
     * \param image is a 2D image with unsgned char type
     */
    void setImage( vtkImageData* image );
    /*!
     * \brief setStartPoints sets the initial points of a closed
     * contour which will be optimized. The points must be in
     * incidental form of the contour.
     * \param points is a vector of 2D points which will be optimized
     */
    void setStartPoints( std::vector<cv::Point2d> points );

    /*!
     * \brief setElasticity sets a factor \p alpha for the elasticity of
     * the contour. If this factor is high the contour will try to minimize
     * its length and tries to tighten and contract.
     * If the factor is low the contour is more elastic.
     */
    void setElasticity( double alpha );

    /*!
     * \brief setStiffness sets the stiffness while optimization of the
     * contour. If this value is high, the contour won't form into
     * hard edges. It will try to keep the surface as streight as possible
     */
    void setStiffness( double beta );

    /*!
     * \brief setIterationSpeed sets the step size of the optimization
     * iteration. This will lead to faster convergence but if this value
     * is to high, the algorithm gets unstable and maybe won't converge.
     */
    void setIterationSpeed( double gamma );

    /*!
     * \brief setEdgeSoothingSigma sets the smoothing of the gradient image.
     * This smooths the gradients and broadens the area of the gradient
     * trajectories.
     * \param sigma if the value is higher the smoothing is stronger
     */
    void setEdgeSoothingSigma( double sigma );

    /*!
     * \brief setMaxIterations sets the maximum number of iterations
     * made to optimize the contour
     */
    void setMaxIterations( unsigned int iterations );

    /*!
     * \brief compute initializes the optimization process and
     * optimizes the curve with the given parameters.
     * \return returns the optimized contour points
     */
    std::vector<cv::Point2d> compute();

    /*!
     * \brief init must be called before using the manual step function.
     * This initializes all needed variables for the contour optimization
     * process.
     */
    void init();

    /*!
     * \brief step computes one step of the optimzation process and returns
     * the new position of the contour. To return the points they will be
     * converted from internal format to the returned format. This will take
     * time and will be slower than computing multiple steps with compute
     * \return new points after one step of the optimization
     */
    std::vector<cv::Point2d> step();

protected:

    /*!
     * \brief iterationStep computes one internal optimzation step
     */
    void iterationStep();

    /*!
     * \brief createP creates the system matrix for optimization
     */
    void createP();

    /*!
     * \brief sampleImage returns the gradient for all given x/y positions
     * given by \p x and \p y. \p gradientType can be 0 or 1. 0 will lead
     * to return the gradient in x direction, 1 in y direction.
     */
    cv::Mat1d sampleImage( cv::Mat1d x, cv::Mat1d y, int gradientType );

    typedef itk::Image<float, 2> FloatImageType;
    typedef itk::Image<itk::CovariantVector<float, 2>, 2> OutputImageType;

    itk::GradientRecursiveGaussianImageFilter<FloatImageType, OutputImageType >::Pointer m_gradientImageFilter;
    // internal pointer to gradient image
    OutputImageType::Pointer m_gradientImage;

    //! System matrix
    cv::Mat1d P;

    //! \see setElasticity();
    double m_elasticity;
    //! \see setStiffness();
    double m_stiffness;
    //! \see setIterationSpeed();
    double m_speed;
    //! \see setEdgeSoothingSigma();
    double m_smoothingSigma;

    //! \See setIterationSpeed();
    unsigned int m_iterations;

    //! holds x values of all points which will be optimized
    cv::Mat1d m_px;
    //! holds y values of all points which will be optimized
    cv::Mat1d m_py;
};

#endif // ACTIVE_CONTOUR_H
