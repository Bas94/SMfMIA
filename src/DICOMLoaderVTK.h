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
	vtkSmartPointer<vtkImageData> loadDICOM(std::string filename);

	vtkSmartPointer<vtkImageData> loadDICOMSeries(std::string directory);
};

