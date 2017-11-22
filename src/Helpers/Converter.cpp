#include "Helpers/Converter.h"
#include <itkVTKImageToImageFilter.h>

void Converter::ConvertVTKToITK(void * pimg, vtkSmartPointer<vtkImageData> imageData)
{
	//typedef itk::Image< PixelType, Dimension > ImageType;

	typedef itk::VTKImageToImageFilter< ImageType> VTKTOITKFilterType;
	VTKTOITKFilterType::Pointer filter = VTKTOITKFilterType::New();
	filter->SetInput(imageData);

	try
	{
		filter->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}

	ImageType::Pointer *outITKImage = (ImageType::Pointer *) pimg;
	*outITKImage = filter->GetOutput();
	return;
}

vtkSmartPointer<vtkImageData> Converter::ConvertITKToVTK(ImageType::Pointer itkImagePointer)
{
	ConnectorType::Pointer connector = ConnectorType::New();

	connector->SetInput(itkImagePointer);
	connector->Update();

	vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
	vtkImage->DeepCopy(connector->GetOutput());

	return vtkImage;
}