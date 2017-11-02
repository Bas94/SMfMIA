#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "DICOMLoaderVTK.h"


int main( int argc, char** argv )	
{
	// TODO: global path??
	std::string inputFilename = "c:\\Users\\lifak\\Documents\\Studium\\09_Master_Wi1718\\SM-MIA\\data\\p01\\0_pre\\00_data\\export0001.dcm";
	std::string directory = "c:\\Users\\lifak\\Documents\\Studium\\09_Master_Wi1718\\SM-MIA\\data\\p01\\0_pre\\00_data";
	
	DICOMLoaderVTK::loadDICOM(inputFilename);

	DICOMLoaderVTK::loadDICOMSeries(directory);

    return 0;
}
