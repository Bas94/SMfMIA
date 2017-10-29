#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include <windows.h>
#include <string.h>
#include <iostream>
#include <shlobj.h>   

#include "myVTKInteractorStyle.h"
#include "DICOMLoaderVTK.h"

// defines which handle read and display of a single DICOM Image or a DICOM Series
#define ONE_DICOM  false
#define SERIES_DICOM  true

// Function that display and enable to interact with DICOMs
void displayImages(vtkSmartPointer<vtkImageData> imageData)
{
	// Visualize
	vtkSmartPointer<vtkImageViewer2> imageViewer =
		vtkSmartPointer<vtkImageViewer2>::New();
	imageViewer->SetInputData(imageData);
	//imageViewer->SetInputConnection(reader->GetOutputPort());		

	// Creats an interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	// Usage of own InteractionStyle
	vtkSmartPointer<myVTKInteractorStyle> myInteractorStyle =
		vtkSmartPointer<myVTKInteractorStyle>::New();
	myInteractorStyle->SetImageViewer(imageViewer);

	imageViewer->SetupInteractor(renderWindowInteractor);

	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

	imageViewer->SetSize(800, 600);
	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();
	renderWindowInteractor->Start();
}

// Function that opens a dialoge window to choose a DICOM file
std::string openFilename(char *filter = "*.dcm", HWND owner = NULL) //"All Files (*.*)\0*.*\0", HWND owner = NULL)
{	
	OPENFILENAMEA ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrInitialDir = "";
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;  ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if (GetOpenFileNameA(&ofn))
		fileNameStr = fileName;
	return fileNameStr;
}

// Function to browse the folder with the DICOM Series
// TODO: set another root folder
std::string openFolder(HWND hwnd = NULL, LPCTSTR szCurrent = NULL, LPTSTR szPath = new TCHAR[MAX_PATH])
{
	BROWSEINFO  bi = { 0 };
	LPITEMIDLIST pidl;
	TCHAR        szDisplay[MAX_PATH];

	LPVOID pvReserved = NULL;
	CoInitialize(pvReserved);
	
	bi.hwndOwner = hwnd;
	bi.pszDisplayName = szDisplay;
	bi.lpszTitle = TEXT("Please choose a folder.");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;	
	bi.lParam = (LPARAM)szCurrent;

	pidl = SHBrowseForFolder(&bi);

    SHGetPathFromIDList(pidl, szPath);

	CoUninitialize();
	return szPath;
}

int main(int argc, char** argv)
{
	std::string inputFilename;
	std::string directory;
	if (ONE_DICOM)
	{
        inputFilename = openFilename();
		vtkSmartPointer<vtkImageData> imageData = DICOMLoaderVTK::loadDICOM(inputFilename);
		displayImages(imageData);
	}

	if (SERIES_DICOM)
	{
        directory = openFolder();
		
		vtkSmartPointer<vtkImageData> imageData = DICOMLoaderVTK::loadDICOMSeries(directory);
		displayImages(imageData);
	}

    return 0;
}
