#ifndef __DICOM_LOADER_H__
#define __DICOM_LOADER_H__

#include <itkImage.h>
#include <stdint.h>
#include <string>

namespace DICOMLoader
{
    /*!
     * \brief loadDICOM is a simpe function to load one slice from a DICOM series
     * \param filename is the name of the slice
     * \return the 2D slice in itk image format
     *         NOTE: if no image was found or other errors appeared this
     *         function will return a NULL pointer
     */
    itk::Image< int32_t, 2 >::Pointer loadDICOM( std::string filename );

    /*!
     * \brief loadDICOMSeries loads a whole DICOM series from a directory
     * \param directory where all series dcm files are placed
     * \return a 3D Volume of the series as itk image format
     *         NOTE: if no image was found or other errors appeared this
     *         function will return a NULL pointer
     */
    itk::Image< int32_t, 3 >::Pointer loadDICOMSeries( std::string directory );
}

#endif // __DICOM_LOADER_H__
