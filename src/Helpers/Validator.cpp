#include "Validator.h"

namespace Validator
{

double diceCoeff2DSlice( MaskType2D::Pointer mask, MaskType2D::Pointer groundtruth )
{
    // check if mask and groudntruth sizes match
    int width = mask->GetLargestPossibleRegion().GetSize()[0];
    int height = mask->GetLargestPossibleRegion().GetSize()[1];
    if( width != groundtruth->GetLargestPossibleRegion().GetSize()[0] ||
        height != groundtruth->GetLargestPossibleRegion().GetSize()[1] )
    {
        std::cerr << "diceCoeff2DSlice: mask and groundtruth must be same sice!" << std::endl;
        return -1;
    }


    // count mask and groundtruth pixels and how many they have in common
    size_t maskCnt = 0;
    size_t groundtruthCnt = 0;
    size_t maskAndGroundtruthCnt = 0;
    MaskType2D::IndexType index;
    for( int y = 0; y < height; ++y )
    {
        for( int x = 0; x < width; ++x )
        {
            index[0] = x;
            index[1] = y;
            bool maskIsSet = static_cast<bool>( mask->GetPixel( index ) );
            bool groundtruthIsSet = static_cast<bool>( groundtruth->GetPixel( index ) );
            if( maskIsSet ) maskCnt++;
            if( groundtruthIsSet ) groundtruthCnt++;
            if( groundtruthIsSet && maskIsSet ) maskAndGroundtruthCnt++;
        }
    }

    // compute dice coefficent
    return static_cast<double>( 2 * maskAndGroundtruthCnt ) / ( maskCnt + groundtruthCnt );
}

double averageEuclideanContourDistance( Contour const & contour, Contour const & groundtruth )
{
    double avgMinDist = 0;
    // TODO: make this less dumb, this currently has O(n^2) run complexity
    // use Kd-Tree or something like this .. vtkKdTree??
    for( size_t i = 0; i < contour.size(); ++i )
    {
        double minDistSqr = std::numeric_limits<double>::max();
        for( size_t j = 0; j < groundtruth.size(); ++j )
        {
            minDistSqr = std::min( cv::norm( cv::Vec2d( contour[i] - groundtruth[j] ), cv::NORM_L2SQR ), minDistSqr );
        }
        avgMinDist += sqrt( minDistSqr );
    }

    return avgMinDist / contour.size();
}

} // namespace Validator
