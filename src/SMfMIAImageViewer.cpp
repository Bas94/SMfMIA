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

vtkStandardNewMacro(SMfMIAImageViewer);

SMfMIAImageViewer::SMfMIAImageViewer()
    : lookupTable( vtkSmartPointer<vtkLookupTable>::New() )
    , mapColor( vtkSmartPointer<vtkImageMapToColors>::New() )
    , m_maskActor( vtkSmartPointer<vtkImageActor>::New() )
    , m_mask( NULL )
{
}

SMfMIAImageViewer::~SMfMIAImageViewer()
{
}

void SMfMIAImageViewer::SetMask( vtkSmartPointer<vtkImageData> mask )
{
    m_mask = mask;

    //map input image for use with imageblend
    lookupTable->SetNumberOfTableValues(2);
    lookupTable->SetRange( mask->GetScalarRange()[0],
                           mask->GetScalarRange()[1] );
    lookupTable->SetTableValue( mask->GetScalarRange()[0], 0.0, 0.0, 0.0, 0.0 ); //label 0 is transparent
    lookupTable->SetTableValue( mask->GetScalarRange()[1], 0.3, 0.0, 0.0, 0.1 ); //label 1 is opaque and green
    lookupTable->Build();

    mapColor->SetLookupTable( lookupTable );
    mapColor->SetInputData( m_mask );
    mapColor->PassAlphaToOutputOn();

    m_maskActor->SetInputData( mapColor->GetOutput() );
}

void SMfMIAImageViewer::UpdateDisplayExtent()
{
    this->Superclass::UpdateDisplayExtent();

    vtkAlgorithm *input = this->GetInputAlgorithm();
    if( !input || !this->m_maskActor )
    {
        return;
    }

    input->UpdateInformation();
    vtkInformation* outInfo = input->GetOutputInformation(0);
    int *w_ext = outInfo->Get(
                vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());

    switch( this->SliceOrientation )
    {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
        this->m_maskActor->SetDisplayExtent(
                    w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice);
        break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
        this->m_maskActor->SetDisplayExtent(
                    w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5]);
        break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
        this->m_maskActor->SetDisplayExtent(
                    this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
        break;
    }
}

void SMfMIAImageViewer::InstallPipeline()
{
    this->Superclass::InstallPipeline();

    if( this->Renderer && this->m_maskActor )
    {
        this->Renderer->AddViewProp( this->m_maskActor );
    }

    if( this->ImageActor && this->WindowLevel )
    {
        this->m_maskActor->GetMapper()->SetInputConnection(
                    this->mapColor->GetOutputPort());
    }
}

void SMfMIAImageViewer::UnInstallPipeline()
{
    this->Superclass::UnInstallPipeline();

    if( this->Renderer && this->m_maskActor )
    {
        this->Renderer->RemoveViewProp( this->m_maskActor );
    }
}

