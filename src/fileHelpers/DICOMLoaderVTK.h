#pragma once
#include <string>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "myVTKInteractorStyle.h"

namespace DICOMLoaderVTK
{
    /*!
     * \brief loadDICOM loads a single 2D DICOM slice
     * \param filename is a path to the DICOM file which we want to load
     * \return returns a 2D image data filled with the 2D DICOM slice
     */
	vtkSmartPointer<vtkImageData> loadDICOM(std::string filename);

    /*!
     * \brief loadDICOMSeries loads a DICOM series out of a given directory
     * \param directory is the directory where the DICOM series is loaded from
     * \return returns 3D image data filled with the DICOM series
     */
    vtkSmartPointer<vtkImageData> loadDICOMSeries( std::string directory );
};

