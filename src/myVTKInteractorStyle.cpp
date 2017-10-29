#include "myVTKInteractorStyle.h"

#include <vtkObjectFactory.h>
//myVTKInteractorStyle* myVTKInteractorStyle::New() {
//	return new myVTKInteractorStyle;
//}

vtkStandardNewMacro(myVTKInteractorStyle);

void myVTKInteractorStyle::SetImageViewer(vtkImageViewer2* imageViewer) 
{
		_ImageViewer = imageViewer;
		_MinSlice = imageViewer->GetSliceMin();
		_MaxSlice = imageViewer->GetSliceMax();
		_Slice = _MinSlice;
}

void myVTKInteractorStyle::MoveSliceForward() 
{
	if (_Slice < _MaxSlice) 
	{
		_Slice += 1;
		_ImageViewer->SetSlice(_Slice);
		_ImageViewer->Render();
	}
}

void myVTKInteractorStyle::MoveSliceBackward() 
{
	if (_Slice > _MinSlice) 
	{
		_Slice -= 1;
		_ImageViewer->SetSlice(_Slice);
		_ImageViewer->Render();
	}
}

void myVTKInteractorStyle::OnKeyDown()
{
	std::string key = this->GetInteractor()->GetKeySym();
	if (key.compare("Up") == 0) 
	{
		MoveSliceForward();
	}
	else if (key.compare("Down") == 0) 
	{
		MoveSliceBackward();
	}
	// forward event
	vtkInteractorStyleImage::OnKeyDown();
}

void myVTKInteractorStyle::OnMouseWheelForward() 
{
	MoveSliceForward();
	// don't forward events, otherwise the image will be zoomed 
	// in case another interactorstyle is used (e.g. trackballstyle, ...)
	// vtkInteractorStyleImage::OnMouseWheelForward();
}

void myVTKInteractorStyle::OnMouseWheelBackward()
{
	if (_Slice > _MinSlice) 
	{
		MoveSliceBackward();
	}
	// don't forward events, otherwise the image will be zoomed 
	// in case another interactorstyle is used (e.g. trackballstyle, ...)
	// vtkInteractorStyleImage::OnMouseWheelBackward();
}
