#ifndef CONTOUR_FROM_MASK_H
#define CONTOUR_FROM_MASK_H

#include <opencv2/core.hpp>
#include <vtkImageData.h>
#include <vector>

namespace ContourFromMask
{

    std::vector<cv::Point2d> compute( vtkImageData* mask, int zSLice );

    std::vector<cv::Point2d> simplify( std::vector<cv::Point2d> const & contour, double eps );

    std::vector<cv::Point2d> resample( std::vector<cv::Point2d> const & contour, size_t numSamplePoints );

} // namespace ContourFromMask

#endif // CONTOUR_FROM_MASK_H
