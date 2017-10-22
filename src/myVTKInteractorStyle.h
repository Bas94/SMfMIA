#pragma once
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkTextMapper.h>
#include <vtkRenderWindowInteractor.h>

class myVTKInteractorStyle : public vtkInteractorStyleImage
{
public:
	static myVTKInteractorStyle* New();
	vtkTypeMacro(myVTKInteractorStyle, vtkInteractorStyleImage);

protected:
	vtkImageViewer2* _ImageViewer;
	vtkTextMapper* _StatusMapper;
	int _Slice;
	int _MinSlice;
	int _MaxSlice;

public:
	void SetImageViewer(vtkImageViewer2* imageViewer);

	

protected:
	void MoveSliceForward();

	void MoveSliceBackward();

	virtual void OnKeyDown(){

	std::string key = this->GetInteractor()->GetKeySym();
	if (key.compare("Up") == 0) {
		//cout << "Up arrow key was pressed." << endl;
		MoveSliceForward();
	}
	else if (key.compare("Down") == 0) {
		//cout << "Down arrow key was pressed." << endl;
		MoveSliceBackward();
	}
	// forward event
	vtkInteractorStyleImage::OnKeyDown();
}
	virtual void OnMouseWheelForward() {
		//std::cout << "Scrolled mouse wheel forward." << std::endl;
		MoveSliceForward();
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelForward();
	}
	virtual void OnMouseWheelBackward()
	{
		//std::cout << "Scrolled mouse wheel backward." << std::endl;
		if (_Slice > _MinSlice) {
			MoveSliceBackward();
		}
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelBackward();
	}
	
};

