#include "Converter.h"
#include <itkVTKImageToImageFilter.h>

itk::VTKImageToImageFilter<itk::Image< PixelType, Dimension>>::Pointer ConvertVTKToITK(vtkSmartPointer<vtkImageData> imageData)
{
	typedef itk::Image< PixelType, Dimension > ImageType;

	typedef itk::VTKImageToImageFilter< ImageType > VTKTOITKFilterType;
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

	itk::Image<PixelType, Dimension>::Pointer outITKImage = itk::Image<PixelType, Dimension>::New();
	outITKImage = filter->GetOutput();
	return filter;
}