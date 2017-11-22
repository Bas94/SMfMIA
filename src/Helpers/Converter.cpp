#include "Helpers/Converter.h"
#include <itkVTKImageToImageFilter.h>

void Converter::ConvertVTKToITK(void * pimg, vtkSmartPointer<vtkImageData> imageData)
{
	//typedef itk::Image< PixelType, Dimension > ImageType;

	typedef itk::VTKImageToImageFilter< ImageTypeConv > VTKTOITKFilterType;
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
