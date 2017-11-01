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
{
}

SMfMIAImageViewer::~SMfMIAImageViewer()
{
}

void SMfMIAImageViewer::AddMask( vtkSmartPointer<vtkImageData> mask,
                                 double colR /*= 0.3*/,
                                 double colG /*= 0.0*/,
                                 double colB /*= 0.0*/,
                                 double colAlpha /*= 0.2*/ )
{
    this->Masks.push_back( mask );

    //map input image for use with imageblend
    vtkSmartPointer<vtkLookupTable> lookupTable =
            vtkSmartPointer<vtkLookupTable>::New();

    lookupTable->SetNumberOfTableValues(2);
    lookupTable->SetRange( mask->GetScalarRange()[0],
                           mask->GetScalarRange()[1] );
    lookupTable->SetTableValue( mask->GetScalarRange()[0], 0.0, 0.0, 0.0, 0.0 ); //label 0 is transparent
    lookupTable->SetTableValue( mask->GetScalarRange()[1], colR, colG, colB, colAlpha ); //label 1 is colored
    lookupTable->Build();
    this->LookupTables.push_back( lookupTable );

    vtkSmartPointer<vtkImageMapToColors> mapColor =
            vtkSmartPointer<vtkImageMapToColors>::New();

    mapColor->SetLookupTable( lookupTable );
    mapColor->SetInputData( mask );
    mapColor->PassAlphaToOutputOn();
    this->MapColors.push_back( mapColor );

    vtkSmartPointer<vtkImageActor> maskActor =
            vtkSmartPointer<vtkImageActor>::New();

    maskActor->SetInputData( mapColor->GetOutput() );
    maskActor->GetMapper()->SetInputConnection( mapColor->GetOutputPort() );
    this->MaskActors.push_back( maskActor );

    if( this->Renderer )
    {
        this->Renderer->AddViewProp( maskActor );
    }
}

void SMfMIAImageViewer::UpdateDisplayExtent()
{
    this->Superclass::UpdateDisplayExtent();

    vtkAlgorithm *input = this->GetInputAlgorithm();
    if( !input || this->MaskActors.empty() )
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
    this->Superclass::InstallPipeline();

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
    this->Superclass::UnInstallPipeline();

    if( this->Renderer && !this->MaskActors.empty() )
    {
        for( size_t i = 0; i < this->MaskActors.size(); ++i )
        {
            this->Renderer->RemoveViewProp( this->MaskActors[i] );
        }
    }
}

