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

	virtual void OnKeyDown();

	virtual void OnMouseWheelForward();
	virtual void OnMouseWheelBackward();
	
};

