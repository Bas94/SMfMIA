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

MaskType2D::Pointer computeITK( Contour const & contour, int *dim )
{
    cv::Mat3b mask = compute( contour, dim );

    MaskType2D::RegionType region;
    MaskType2D::IndexType start;
    start[0] = 0;
    start[1] = 0;

    MaskType2D::SizeType size;
    size[0] = dim[0];
    size[1] = dim[1];

    region.SetSize(size);
    region.SetIndex(start);

    MaskType2D::Pointer image = MaskType2D::New();
    image->SetRegions(region);
    image->Allocate();

    MaskType2D::IndexType index;
    for( int y = 0; y < dim[1]; ++y )
    {
        index[1] = y;
        for( int x = 0; x < dim[0]; ++x )
        {
            index[0] = x;
            image->SetPixel( index, mask( y, x )[0] > 0 );
        }
    }

    return image;
}

} // namespace ContourToMask
