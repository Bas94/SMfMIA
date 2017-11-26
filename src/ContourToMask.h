#ifndef CONTOUR_TO_MASK_H
#define CONTOUR_TO_MASK_H

#include <opencv2/core.hpp>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vector>
#include <ActiveContour.h>

namespace ContourToMask
{
    cv::Mat3b compute( Contour const & contour , int *dim );
} // namespace ContourToMask

#endif // CONTOUR_TO_MASK_H
