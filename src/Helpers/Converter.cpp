#include "Helpers/Converter.h"

vtkSmartPointer<vtkImageData> Converter::ConvertITKToVTK(ImageType::Pointer itkImagePointer)
{
	ConnectorType::Pointer connector = ConnectorType::New();

	connector->SetInput(itkImagePointer);
	connector->Update();

	vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
	vtkImage->DeepCopy(connector->GetOutput());

	return vtkImage;
}
