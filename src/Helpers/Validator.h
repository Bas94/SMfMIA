#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "Helpers/TypeDefinitions.h"

namespace Validator
{
    double diceCoeff2DSlice( MaskType2D::Pointer mask, MaskType2D::Pointer groundtruth );
    double averageEuclideanContourDistance( Contour const & contour, Contour const & groundtruth );
}

#endif // VALIDATOR_H
