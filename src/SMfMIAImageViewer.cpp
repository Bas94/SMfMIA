#include "SMfMIAImageViewer.h"

#include <vtkObjectFactory.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkRenderer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>

// create the New() function of this class as needed by the VTK library
vtkStandardNewMacro(SMfMIAImageViewer);

SMfMIAImageViewer::SMfMIAImageViewer()
    : vtkResliceImageViewer()
{
}

SMfMIAImageViewer::~SMfMIAImageViewer()
{
}

void SMfMIAImageViewer::Show(vtkSmartPointer<vtkImageData > imageData)
{
	// Visualize
	vtkSmartPointer<SMfMIAImageViewer> imageViewer =
		vtkSmartPointer<SMfMIAImageViewer>::New();
	// Creats an interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	// Usage of own InteractionStyle
	vtkSmartPointer<SMfMIAInteractorStyle> myInteractorStyle =
		vtkSmartPointer<SMfMIAInteractorStyle>::New();
	myInteractorStyle->SetImageViewer(imageViewer);

	imageViewer->SetupInteractor(renderWindowInteractor);
	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

	imageViewer->SetInputData(imageData);

	//imageViewer->GetRenderer()->SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i]);

	imageViewer->SetSize(800, 600);
	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();
	imageViewer->SetSliceOrientationToXY();
	renderWindowInteractor->Start();
}

void SMfMIAImageViewer::Render()
{
	/*if (m_ImageData.size() > 1)
	{

	}
	else
	{*/
		vtkResliceImageViewer::Render();
	//}
}


void SMfMIAImageViewer::AddMask( vtkSmartPointer<vtkImageData> mask,
                                 double colR /*= 0.3*/,
                                 double colG /*= 0.0*/,
                                 double colB /*= 0.0*/,
                                 double colAlpha /*= 0.2*/ )
{
    this->Masks.push_back( mask );

    // create a look up table which maps our 0 and 1 values to actual colors
    vtkSmartPointer<vtkLookupTable> lookupTable =
            vtkSmartPointer<vtkLookupTable>::New();
    lookupTable->SetNumberOfTableValues(2);
    lookupTable->SetRange( mask->GetScalarRange()[0],
                           mask->GetScalarRange()[1] );
    lookupTable->SetTableValue( mask->GetScalarRange()[0], 0.0, 0.0, 0.0, 0.0 ); //label 0 is transparent
    lookupTable->SetTableValue( mask->GetScalarRange()[1], colR, colG, colB, colAlpha ); //label 1 is colored
    lookupTable->Build();
    this->LookupTables.push_back( lookupTable );

    // create the rendering part which calls for every value in the mask
    // our look up table
    vtkSmartPointer<vtkImageMapToColors> mapColor =
            vtkSmartPointer<vtkImageMapToColors>::New();
    mapColor->SetLookupTable( lookupTable );
    mapColor->SetInputData( mask );
    mapColor->PassAlphaToOutputOn();
    this->MapColors.push_back( mapColor );

    // bind the mapColor object to an actor which can be called by the renderer
    vtkSmartPointer<vtkImageActor> maskActor =
            vtkSmartPointer<vtkImageActor>::New();
    maskActor->SetInputData( mapColor->GetOutput() );
    maskActor->GetMapper()->SetInputConnection( mapColor->GetOutputPort() );
    this->MaskActors.push_back( maskActor );

    // add everything to the renderer if possible / renderer exists
    if( this->Renderer )
    {
        this->Renderer->AddViewProp( maskActor );
    }
}

void SMfMIAImageViewer::UpdateDisplayExtent()
{
    // do all updates of the parent class before we do anything
    this->Superclass::UpdateDisplayExtent();

    // can we do something?
    vtkAlgorithm *input = this->GetInputAlgorithm();
    if( !input || this->MaskActors.empty() )
    {
        return;
    }

    // get needed extend information
    input->UpdateInformation();
    vtkInformation* outInfo = input->GetOutputInformation(0);
    int *w_ext = outInfo->Get(
                vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());

    // updated extend based on current set orientation
    switch( this->SliceOrientation )
    {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->MaskActors[i]->SetDisplayExtent(
                        w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice);
        }
        break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->MaskActors[i]->SetDisplayExtent(
                        w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5]);
        }
        break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->MaskActors[i]->SetDisplayExtent(
                        this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
        }
        break;
    }
}

void SMfMIAImageViewer::InstallPipeline()
{
    // install everything of the parent class before we do anything
    this->Superclass::InstallPipeline();

    // add our actors which hold our overlay renderers
    if( this->Renderer && !this->MaskActors.empty() )
    {
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->Renderer->AddViewProp( this->MaskActors[i] );
        }
    }
}

void SMfMIAImageViewer::UnInstallPipeline()
{
    // remove everything of the parent class before we do anything
    this->Superclass::UnInstallPipeline();

    // remove our actors which hold our overlay renderers
    if( this->Renderer && !this->MaskActors.empty() )
    {
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->Renderer->RemoveViewProp( this->MaskActors[i] );
        }
    }
}

