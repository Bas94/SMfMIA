#ifndef SMFMIA_IMAGE_VIEWER
#define SMFMIA_IMAGE_VIEWER

#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vector>

class vtkLookupTable;
class vtkImageMapToColors;

class VTKINTERACTIONIMAGE_EXPORT SMfMIAImageViewer : public vtkResliceImageViewer
{
public:
    static SMfMIAImageViewer *New();
    vtkTypeMacro(SMfMIAImageViewer,vtkResliceImageViewer);

    void AddMask( vtkSmartPointer<vtkImageData> mask,
                  double colR = 0.3,
                  double colG = 0.0,
                  double colB = 0.0,
                  double colAlpha = 0.2 );
    virtual void UpdateDisplayExtent();

protected:
    SMfMIAImageViewer();
    ~SMfMIAImageViewer();

    virtual void InstallPipeline();
    virtual void UnInstallPipeline();

    std::vector< vtkSmartPointer<vtkImageData> > Masks;
    std::vector< vtkSmartPointer<vtkImageActor> > MaskActors;
    std::vector< vtkSmartPointer<vtkLookupTable> > LookupTables;
    std::vector< vtkSmartPointer<vtkImageMapToColors> > MapColors;
};

#endif // SMFMIA_IMAGE_VIEWER
