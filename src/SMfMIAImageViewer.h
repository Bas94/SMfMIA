#ifndef SMFMIA_IMAGE_VIEWER
#define SMFMIA_IMAGE_VIEWER

#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>

class vtkLookupTable;
class vtkImageMapToColors;

class VTKINTERACTIONIMAGE_EXPORT SMfMIAImageViewer : public vtkResliceImageViewer
{
public:
    static SMfMIAImageViewer *New();
    vtkTypeMacro(SMfMIAImageViewer,vtkResliceImageViewer);

    void SetMask( vtkSmartPointer<vtkImageData> mask,
                  double colR = 0.3,
                  double colG = 0.0,
                  double colB = 0.0,
                  double colAlpha = 0.1 );
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
