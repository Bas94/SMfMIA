#include "myVTKInteractorStyle.h"

#include <vtkObjectFactory.h>
//myVTKInteractorStyle* myVTKInteractorStyle::New() {
//	return new myVTKInteractorStyle;
//}

vtkStandardNewMacro(myVTKInteractorStyle);

void myVTKInteractorStyle::SetImageViewer(vtkImageViewer2* imageViewer) {
		_ImageViewer = imageViewer;
		_MinSlice = imageViewer->GetSliceMin();
		_MaxSlice = imageViewer->GetSliceMax();
		_Slice = _MinSlice;
		cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
	}

	void myVTKInteractorStyle::MoveSliceForward() {
		if (_Slice < _MaxSlice) {
			_Slice += 1;
			cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
		//	std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
		//	_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}

	void myVTKInteractorStyle::MoveSliceBackward() {
		if (_Slice > _MinSlice) {
			_Slice -= 1;
			cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
			//std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			//_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}
