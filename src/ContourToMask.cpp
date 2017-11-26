#include "ContourToMask.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace ContourToMask
{

cv::Mat3b compute( Contour const & contour , int *dim )
{
    cv::Mat3b mask( dim[1], dim[0], cv::Vec3b( 0, 0, 0 ) );
    std::vector<std::vector<cv::Point> > fillContAll;
    std::vector<cv::Point> p( contour.size() );
    for( size_t i = 0; i < contour.size(); ++i )
    {
        p[i] = contour[i];
    }
    fillContAll.push_back( p );
    cv::fillPoly( mask, fillContAll, cv::Scalar( 255, 255, 255 ) );

    return mask;
//    return maskData;
}

} // namespace ContourToMask
