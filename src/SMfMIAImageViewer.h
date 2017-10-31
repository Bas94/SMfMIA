#ifndef SMFMIA_IMAGE_VIEWER
#define SMFMIA_IMAGE_VIEWER

#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vtkImageBlend.h>

class vtkLookupTable;
class vtkImageMapToColors;

class VTKINTERACTIONIMAGE_EXPORT SMfMIAImageViewer : public vtkResliceImageViewer
{
public:
    static SMfMIAImageViewer *New();
    vtkTypeMacro(SMfMIAImageViewer,vtkResliceImageViewer);

    void SetMask( vtkSmartPointer<vtkImageData> mask );
    virtual void UpdateDisplayExtent();

protected:
    SMfMIAImageViewer();
    ~SMfMIAImageViewer();

    virtual void InstallPipeline();
    virtual void UnInstallPipeline();

    vtkSmartPointer<vtkImageActor> m_maskActor;
    vtkSmartPointer<vtkImageData> m_mask;

    vtkSmartPointer<vtkLookupTable> lookupTable;
    vtkSmartPointer<vtkImageMapToColors> mapColor;
};

#endif // SMFMIA_IMAGE_VIEWER
