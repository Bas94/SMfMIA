#ifndef CONTOUR_TO_MASK_H
#define CONTOUR_TO_MASK_H

#include <opencv2/core.hpp>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vector>
#include "Helpers/TypeDefinitions.h"

namespace ContourToMask
{
    cv::Mat3b compute( Contour const & contour, int *dim );
    MaskType2D::Pointer computeITK( Contour const & contour, int *dim );

    std::vector<cv::Mat3b> compute( std::vector<std::vector<Contour> > const &contour, int *dim );
    MaskType::Pointer computeITK( std::vector<std::vector<Contour> > const &contour, int *dim );
} // namespace ContourToMask

#endif // CONTOUR_TO_MASK_H
