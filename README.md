# SegmentationMethodsForMedicalImageAnalysis

This is a project for the Lecture Segmentation Methods for Medical Image Analysis. The Goal is to create a robust segmentation algorithm to find livers within MRT scans.

## license
This project is under MIT license (see LICENSE file in this project)

## getting started
For initializing under Windows you need:
  * cmake
  * cmake-gui
  * Visual Studio 2017
  * git
make sure that git and cmake are within the PATH variable.

For Linux you need:
  * build tools (gcc, g++, linker etc)
  * cmake
  * cmake-gui
  * git
  * also you may want to install ffmpeg dependencies if you need video file support in opencv (libavcodec-devel libavdevice-devel libavfilter-devel libavformat-devel libavresample-devel libavutil-devel libpostproc-devel libswresample-devel libswscale-devel)

To initialise the project just use the init_repo scripts.
On Windows open a cmd and call init_repo.bat
O Linux call init_repo.sh
