#ifndef CONTOUR_FROM_MASK_H
#define CONTOUR_FROM_MASK_H

#include <opencv2/core.hpp>
#include <vtkImageData.h>
#include <vector>
#include "Helpers/TypeDefinitions.h"

namespace ContourFromMask
{
    /*!
     * \brief compute creates a contour from a given mask in XY plane of the
     *        a given z slice
     * \param mask is a bit mask which defines the area, the values of this
     *        mask should be 0 (background) and 1 (foreground).
     *        Also there should be only one component and the mask shouldn't fall
     *        into 2 or more parts.
     * \param zSLice is the z slice index of the mask where the XY is chosen from
     * \return returns vector of points which discribe the outer contour of the mask
     */
    Contour compute( vtkImageData* mask, int zSLice );
    std::vector<Contour> computeWithEdgeFilter(vtkImageData* mask, int zSLice , int sizeThreshold = 40 );

    /*!
     * \brief simplify simplifies a given contour. The simplification is done with
     * Douglas-Peucker-algorithm.
     * \param contour is a given vector of points which discribes a contour
     * \param eps is the maximum error distance a point can have from the resulting
     *        simplified lineto be discarded
     * \return returns the simplified set of points
     */
    Contour simplify( Contour const & contour, double eps );

    /*!
     * \brief resample resamples a given contour, so that all points are equally
     * distriuted over the contour.
     * \param contour the input vector of points which discribe the contour to resample
     * \param numSamplePoints defines the number of points which are returned for the
     * resampled contour
     * \return resampled contour with \p numSamplePoints points
     */
    Contour resample( Contour const & contour, size_t numSamplePoints );

} // namespace ContourFromMask

#endif // CONTOUR_FROM_MASK_H
