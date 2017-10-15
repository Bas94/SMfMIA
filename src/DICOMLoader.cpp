#include "DICOMLoader.h"

#include "itkGDCMImageIO.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesWriter.h"
#include "itkMetaDataObject.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"

namespace DICOMLoader
{


itk::Image< int32_t, 2 >::Pointer loadDICOM( std::string filename )
{
    if( filename.empty() )
    {
        std::cerr << "Filename is empty. Please set a filename." << std::endl;
        return 0;
    }

    typedef itk::Image< int32_t, 2 >      ImageType;
    typedef itk::ImageFileReader< ImageType >       ReaderType;

    ReaderType::Pointer reader = ReaderType::New();

    reader->SetFileName( filename );

    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
        std::cerr << "Exception thrown while loading the image" << std::endl;
        std::cerr << excp << std::endl;
        return 0;
    }

    return reader->GetOutput();
}

itk::Image< int32_t, 3 >::Pointer loadDICOMSeries( std::string directory )
{
    if( directory.empty() )
    {
        std::cerr << "Directory is empty. Please set a directory." << std::endl;
        return 0;
    }

    itk::GDCMImageIO::Pointer gdcmIO = itk::GDCMImageIO::New();
    itk::GDCMSeriesFileNames::Pointer inputNames = itk::GDCMSeriesFileNames::New();
    inputNames->SetInputDirectory( directory );

    const itk::ImageSeriesReader< itk::Image< int32_t, 3 > >::FileNamesContainer & filenames =
            inputNames->GetInputFileNames();

    itk::ImageSeriesReader< itk::Image< int32_t, 3 > >::Pointer reader =
            itk::ImageSeriesReader< itk::Image< int32_t, 3 > >::New();

    reader->SetImageIO( gdcmIO );
    reader->SetFileNames( filenames );

    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
        std::cerr << "Exception thrown while loading the image" << std::endl;
        std::cerr << excp << std::endl;
        return 0;
    }

    return reader->GetOutput();
}

} // namespace DICOMLoader
