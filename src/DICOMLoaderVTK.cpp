#include "DICOMLoaderVTK.h"
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "myVTKInteractorStyle.h"

namespace DICOMLoaderVTK
{
	void loadDICOM(std::string filename) {
		if (filename.empty())
		{
			std::cerr << "Filename is empty. Please set a filename." << std::endl;			
		}

		vtkSmartPointer<vtkDICOMImageReader> reader =
			vtkSmartPointer<vtkDICOMImageReader>::New();

		reader->SetFileName(filename.c_str());

		reader->Update();

		// Visualize
		vtkSmartPointer<vtkImageViewer2> imageViewer =
			vtkSmartPointer<vtkImageViewer2>::New();
		imageViewer->SetInputConnection(reader->GetOutputPort());
		vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
		imageViewer->SetupInteractor(renderWindowInteractor);
		imageViewer->Render();
		imageViewer->GetRenderer()->ResetCamera();
		imageViewer->Render();

		renderWindowInteractor->Start();
	}

	void loadDICOMSeries(std::string directory)
	{
		// Read all the DICOM files in the specified directory.
		vtkSmartPointer<vtkDICOMImageReader> reader =
			vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetDirectoryName(directory.c_str());
		reader->Update();

		// Visualize
		vtkSmartPointer<vtkImageViewer2> imageViewer =
			vtkSmartPointer<vtkImageViewer2>::New();
		imageViewer->SetInputConnection(reader->GetOutputPort());		

		// Creats an interactor
		vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();

		// Usage of own InteractionStyle
		vtkSmartPointer<myVTKInteractorStyle> myInteractorStyle =
			vtkSmartPointer<myVTKInteractorStyle>::New();
		myInteractorStyle->SetImageViewer(imageViewer);

		imageViewer->SetupInteractor(renderWindowInteractor);
		
		renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

		imageViewer->Render();
		imageViewer->GetRenderer()->ResetCamera();
		imageViewer->Render();
		renderWindowInteractor->Start();
	}
};