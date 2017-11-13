#ifndef SMFMIA_IMAGE_VIEWER
#define SMFMIA_IMAGE_VIEWER

#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vector>

class vtkLookupTable;
class vtkImageMapToColors;

/*!
 * \brief The SMfMIAImageViewer class is a from vtkResliceImageViewer
 * derived class which can display masks as overlay above a gray value dataset
 */
class SMfMIAImageViewer : public vtkResliceImageViewer
{
public:
    // VTK design defined new function
    static SMfMIAImageViewer *New();
    // do some library registration and stuff - needed by VTK library design
    vtkTypeMacro(SMfMIAImageViewer,vtkResliceImageViewer);

    /*!
     * \brief AddMask adds a mask overly above the given dataset with
     *        a given color
     * \param mask is a binary image (value are 0 or 1) which has the
     *        same size as the main image dataset
     * \param colR, \p colG and \p colB define the color of the added mask
     * \param colAlpha defines how opaque the mask overlay is
     *        (1.0 = fully opaque, 0.5 = transparent, 0.0 = invisible)
     */
    void AddMask( vtkSmartPointer<vtkImageData> mask,
                  double colR = 0.3,
                  double colG = 0.0,
                  double colB = 0.0,
                  double colAlpha = 0.2 );

protected:
    // constructor to create the object
    // protected because of the own memory handling given by VTK design
    SMfMIAImageViewer();
    // destructor to destroy the object
    // protected because of the own memory handling given by VTK design
    virtual ~SMfMIAImageViewer();

    // installs the needed objects within the pipeline to get an overlay
    virtual void InstallPipeline();
    // removes the needed objects within the pipeline to get an overlay
    virtual void UnInstallPipeline();
    // updates the current viewed slice if another slice was requested
    virtual void UpdateDisplayExtent();

    // all the data needed to view an overlay:

    // all mask data
    std::vector< vtkSmartPointer<vtkImageData> > Masks;
    // all actors which are object to handle the rendering classes
    std::vector< vtkSmartPointer<vtkImageActor> > MaskActors;
    // all look up tables to manage which value gets which color within the mask
    std::vector< vtkSmartPointer<vtkLookupTable> > LookupTables;
    // all color mappers which do the actual rendering step of coloring the mask
    std::vector< vtkSmartPointer<vtkImageMapToColors> > MapColors;
};

#endif // SMFMIA_IMAGE_VIEWER
