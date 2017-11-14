#include "DICOMLoaderVTK.h"

#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace DICOMLoaderVTK
{
	// Function that read in a single DICOM Image
    vtkSmartPointer<vtkImageData> loadDICOM( std::string filename )
	{	
		vtkSmartPointer<vtkDICOMImageReader> reader =
			vtkSmartPointer<vtkDICOMImageReader>::New();

		reader->SetFileName(filename.c_str());
		reader->Update();

		vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();
		return imageData;
	}

	// Function that read in DICOM Series
    vtkSmartPointer<vtkImageData> loadDICOMSeries( std::string directory )
	{
		// Read all the DICOM files in the specified directory.
		vtkSmartPointer<vtkDICOMImageReader> reader =
			vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetDirectoryName(directory.c_str());
		
		// TODO: check if directory contains DICOM Images, but don�t know how...
		
		reader->Update();

		vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();

		return imageData;		
	}

} // namespace DICOMLoaderVTK
