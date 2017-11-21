#ifndef CONTOUR_FROM_MASK_H
#define CONTOUR_FROM_MASK_H

#include <opencv2/core.hpp>
#include <vtkImageData.h>
#include <vector>

namespace ContourFromMask
{

    std::vector<cv::Point2d> compute( vtkImageData* mask, int zSLice );

} // namespace ContourFromMask

#endif // CONTOUR_FROM_MASK_H
