#include "ContourFromMask.h"

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDataArrayIteratorMacro.h>

namespace ContourFromMask
{

/*
 * all possible cases of edges:
 *
 * -------
 * |X|X| |
 * -------
 * |X|X|X| // up / right
 * -------
 * |X|X|X|
 * -------
 *
 * -------
 * | |X|X|
 * -------
 * |X|X|X| // up / left
 * -------
 * |X|X|X|
 * -------
 *
 * -------
 * |X|X|X|
 * -------
 * |X|X|X| // down / right
 * -------
 * |X|X| |
 * -------
 *
 * -------
 * |X|X|X|
 * -------
 * |X|X|X| // down / left
 * -------
 * | |X|X|
 * -------
 *
 * -------
 * |?| |?|
 * -------
 * |X|X|X| // left / right
 * -------
 * |X|X|X|
 * -------
 *
 * -------
 * |X|X|X|
 * -------
 * |X|X|X| // left / right
 * -------
 * |?| |?|
 * -------
 *
 * -------
 * |X|X|?|
 * -------
 * |X|X| | // up / down
 * -------
 * |X|X|?|
 * -------
 *
 * -------
 * |?|X|X|
 * -------
 * | |X|X| // up / down
 * -------
 * |?|X|X|
 * -------
 *
 * -------
 * |?|X|X|
 * -------
 * | |X|X| // up / right
 * -------
 * | | |?|
 * -------
 *
 * -------
 * |X|X|?|
 * -------
 * |X|X| | // up / left
 * -------
 * |?| | |
 * -------
 *
 * -------
 * | | |?|
 * -------
 * | |X|X| // down / right
 * -------
 * |?|X|X|
 * -------
 *
 * -------
 * |?| | |
 * -------
 * |X|X| | // down / left
 * -------
 * |X|X|?|
 * -------
 *
 * -------
 * |X|X| |
 * -------
 * |X|X|X| // down / up
 * -------
 * |X|X| |
 * -------
 *
 * -------
 * | |X|X|
 * -------
 * |X|X|X| // down / up
 * -------
 * | |X|X|
 * -------
 *
 * -------
 * | |X| |
 * -------
 * |X|X|X| // left / right
 * -------
 * |X|X|X|
 * -------
 *
 * -------
 * |X|X|X|
 * -------
 * |X|X|X| // left / right
 * -------
 * | |X| |
 * -------
 */

enum MoveType
{
    MTLeft = 0,
    MTDown = 1,
    MTRight = 2,
    MTUp = 3
};

static void edgeWalk( int x, int y, int z,
                      int finishX, int finishY,
                      MoveType lastMove,
                      vtkImageData* mask,
                      std::vector<cv::Point2d> & foundPoints )
{
    while( x != finishX || y != finishY )
    {
        foundPoints.push_back( cv::Point2d( x, y ) );

        if( !*reinterpret_cast<short*>( mask->GetScalarPointer( x, y, z ) ) )
        {
            return;
        }

        short left  = *reinterpret_cast<short*>( mask->GetScalarPointer( x - 1, y,     z ) );
        short right = *reinterpret_cast<short*>( mask->GetScalarPointer( x + 1, y,     z ) );
        short up    = *reinterpret_cast<short*>( mask->GetScalarPointer( x,     y - 1, z ) );
        short down  = *reinterpret_cast<short*>( mask->GetScalarPointer( x,     y + 1, z ) );

        short leftUp = *reinterpret_cast<short*>( mask->GetScalarPointer( x - 1, y - 1, z ) );
        short rightUp = *reinterpret_cast<short*>( mask->GetScalarPointer( x + 1, y - 1, z ) );
        short leftDown = *reinterpret_cast<short*>( mask->GetScalarPointer( x - 1, y + 1, z ) );
        short rightDown = *reinterpret_cast<short*>( mask->GetScalarPointer( x + 1, y + 1, z ) );

        MoveType possibleMoves[2];

        if( ( ( !left || !right ) && ( up && down ) ) ||
            ( right && !rightUp && !rightDown ) || ( left && !leftUp && !leftDown ) )
        {
            possibleMoves[0] = MTUp;
            possibleMoves[1] = MTDown;
        }
        else if( ( ( !up || !down ) && ( left && right ) ) ||
                 ( up && !rightUp && !leftUp ) || ( down && !rightDown && !leftDown ))
        {
            possibleMoves[0] = MTLeft;
            possibleMoves[1] = MTRight;
        }
        else if( !up && !left )
        {
            possibleMoves[0] = MTDown;
            possibleMoves[1] = MTRight;
        }
        else if( !up && !right )
        {
            possibleMoves[0] = MTDown;
            possibleMoves[1] = MTLeft;
        }
        else if( !down && !left )
        {
            possibleMoves[0] = MTUp;
            possibleMoves[1] = MTRight;
        }
        else if( !down && !right )
        {
            possibleMoves[0] = MTUp;
            possibleMoves[1] = MTLeft;
        }
        else
        {
            if( !leftDown )
            {
                possibleMoves[0] = MTLeft;
                possibleMoves[1] = MTDown;
            }
            else if( !rightDown )
            {
                possibleMoves[0] = MTRight;
                possibleMoves[1] = MTDown;
            }
            else if( !leftUp )
            {
                possibleMoves[0] = MTLeft;
                possibleMoves[1] = MTUp;
            }
            else if( !rightUp )
            {
                possibleMoves[0] = MTRight;
                possibleMoves[1] = MTUp;
            }
            else
            {
                std::cerr << "ContourFromMask: Contour was lost" << std::endl;
                return;
            }
        }

        MoveType move;
        // don't go into the other direction again
        if( ( possibleMoves[0] == MTLeft  && lastMove != MTRight ) ||
            ( possibleMoves[0] == MTRight && lastMove != MTLeft ) ||
            ( possibleMoves[0] == MTUp    && lastMove != MTDown ) ||
            ( possibleMoves[0] == MTDown  && lastMove != MTUp ) )
        {
            move = possibleMoves[0];
        }
        else
        {
            move = possibleMoves[1];
        }

        if( move == MTLeft )
        {
            x -= 1;
        }
        else if( move == MTDown )
        {
            y += 1;
        }
        else if( move == MTRight )
        {
            x += 1;
        }
        else // if( move == MTUp )
        {
            y -= 1;
        }
        lastMove = move;
    }
}

std::vector<cv::Point2d> compute( vtkImageData* mask, int zSlice )
{
    std::vector<cv::Point2d> result;

    bool breaked = false;
    int* dims = mask->GetDimensions();
    assert( 0 <= zSlice && zSlice < dims[2] );
    for( int y = 1; y < dims[1] - 1; ++y )
    {
        for( int x = 1; x < dims[0] - 1; ++x )
        {
            short v = *reinterpret_cast<short*>( mask->GetScalarPointer( x, y, zSlice ) );
            if( v )
            {
                short down = *reinterpret_cast<short*>( mask->GetScalarPointer( x, y + 1, zSlice ) );

                result.push_back( cv::Point2d( x, y) );
                if( !down )
                {
                    edgeWalk( x+1, y, zSlice, x, y, MTRight, mask, result );
                }
                else
                {
                    edgeWalk( x, y+1, zSlice, x, y, MTDown, mask, result );
                }
                breaked = true;
                break;
            }
        }
        if( breaked )
            break;
    }
    return result;
}

static void simplify( size_t first,
                      size_t last,
                      std::vector<cv::Point2d> const & contour,
                      std::vector<size_t> & reducedPoints,
                      double eps )
{
     cv::Point2d const & p1 = contour[first];
     cv::Point2d const & p2 = contour[last];
     double maxDist = 0;
     size_t maxIndex = 0;

     // n^t*(x-p) = distance, for normalized n
     cv::Vec2d n( p1.y-p2.y, p2.x-p1.x );
     n = cv::normalize( n );
     for( size_t i = first + 1; i < last; ++i )
     {
         double dist = fabs( n.dot( contour[i] - p1 ) );
         if( maxDist < dist )
         {
            maxDist = dist;
            maxIndex = i;
         }
     }
     if( maxDist < eps )
         return;

     reducedPoints.push_back( maxIndex );

     simplify( first, maxIndex, contour, reducedPoints, eps );
     simplify( maxIndex, last, contour, reducedPoints, eps );
}

std::vector<cv::Point2d> simplify( std::vector<cv::Point2d> const & contour, double eps )
{
    std::vector<size_t> allowedIndices;
    allowedIndices.push_back( 0 );
    allowedIndices.push_back( contour.size()-1 );
    simplify( 0, contour.size()-1, contour, allowedIndices, eps );

    std::sort( allowedIndices.begin(), allowedIndices.end() );

    std::vector<cv::Point2d> result( allowedIndices.size() );
    for( size_t i = 0; i < allowedIndices.size(); ++i )
    {
         result[i] = contour[allowedIndices[i]];
    }
    return result;
}

std::vector<cv::Point2d> resample( std::vector<cv::Point2d> const &contour, size_t numSamplePoints )
{
    double wholeLength = 0;
    size_t lastI = contour.size()-1;
    for( size_t i = 0; i < contour.size(); ++i )
    {
        wholeLength += cv::norm( contour[i] - contour[lastI] );
        lastI = i;
    }

    double sampleDistance = wholeLength / numSamplePoints;
    size_t p1Index = contour.size()-1;
    size_t p2Index = 0;
    double currDist = 0;
    double currLineDist = cv::norm( contour[p2Index] - contour[p1Index] );
    double lineEndDist = currLineDist;
    std::vector<cv::Point2d> result;
    for( size_t i = 0; i < numSamplePoints; ++i, currDist += sampleDistance )
    {
        while( lineEndDist < currDist )
        {
            p1Index = p2Index;
            p2Index++;
            currLineDist = cv::norm( contour[p2Index] - contour[p1Index] );
            lineEndDist += currLineDist;
        }
        double t = ( lineEndDist - currDist ) / currLineDist;
        result.push_back( contour[p2Index] - t * ( contour[p2Index] - contour[p1Index] ) );
    }
    return result;
}

} // namespace ContourFromMask
