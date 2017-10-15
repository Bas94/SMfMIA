@echo off
setlocal

set project_name=opencv
set version=3.1.0
set file_name=opencv-%version%
set file_name_contrib=opencv_contrib-%version%
set build_root=%cd%
set extern_root=%build_root%
set export_root=%build_root%/install
set bits=64
set build_configuration=Release
set additional_cmake_options=
set cmake_options=-DBUILD_DOCS=0 -DOPENCV_EXTRA_MODULES_PATH=%extern_root%/%file_name_contrib%/modules -DBUILD_opencv_bioinspired=0 -DBUILD_opencv_xfeatures2d=1 -DBUILD_opencv_ximgproc=1 -DBUILD_opencv_xobjdetect=1 -DBUILD_opencv_xphoto=1 -DBUILD_EXAMPLES=0 -DBUILD_FAT_JAVA_LIB=0 -DBUILD_JPEG=1 -DWITH_OPENEXR=0 -DBUILD_PACKAGE=1 -DBUILD_PERF_TESTS=0 -DBUILD_PNG=1 -DBUILD_SHARED_LIBS=0 -DBUILD_TBB=0 -DWITH_IPP=0 -DBUILD_TESTS=0 -DBUILD_WITH_DEBUG_INFO=1 -DBUILD_ZLIB=0 -DBUILD_opencv_apps=0 -DBUILD_opencv_calib3d=1 -DBUILD_opencv_contrib=0 -DBUILD_opencv_legacy=0 -DBUILD_opencv_core=1 -DBUILD_opencv_features2d=1 -DBUILD_opencv_photo=1 -DWITH_CUDA=0 -DWITH_FFMPEG=1 -DWITH_LIB4L=0-DWITH_JPEG=1 -DWITH_JASPER=0 -DWITH_PNG=1 -DWITH_PVAPI=0 -DWITH_NVCUVID=0 -DWITH_OPENCL=0 -DWITH_OPENCLAMDBLAS=0 -DWITH_OPENCLAMDFFT=0 -DWITH_TIFF=1 -DWITH_LIBV4L=0 -DWITH_V4L=0 -DWITH_VTK=0 -DWITH_WEBP=0 -DBUILD_opencv_ml=1 -DBUILD_opencv_flann=1 -DBUILD_opencv_video=1 -DBUILD_opencv_videostab=0 -DBUILD_opencv_videoio=1 -DBUILD_opencv_gpu=0 -DBUILD_opencv_python2=0 -DBUILD_WITH_STATIC_CRT=0 -DBUILD_opencv_stitching=1 -DBUILD_opencv_ts=0

set msgprefix=*** install_%project_name%.bat: 

if exist "%build_root%/%file_name%/" goto install

:unpack
echo %msgprefix% Extracting...
call git clone https://github.com/opencv/opencv.git %file_name%
if errorlevel 1 (
  rmdir %file_name% /s /q
  goto :eof 
)
cd %file_name%
call git checkout %version%
cd ..

call git clone https://github.com/opencv/opencv_contrib.git %file_name_contrib%
if errorlevel 1 (
  rmdir %file_name_contrib% /s /q
  goto :eof 
)
cd %file_name_contrib%
call git checkout %version%

:install
if not exist "%export_root%/include" mkdir "%export_root%/include"
if not exist "%export_root%/lib" mkdir "%export_root%/lib"
if not exist "%export_root%/%file_name%/build_%bits%" mkdir "%export_root%/%file_name%/build_%bits%"
cd "%export_root%/%file_name%/build_%bits%"

:: set all system variables, so we find visual studio
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build" (
set compiler="Visual Studio 15 2017 Win64"
set variableScript="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
) 

call %variableScript% amd64

call cmake -G %compiler% %build_root%/%file_name% %cmake_options% %additional_cmake_options% -DCMAKE_INSTALL_PREFIX="%export_root%" -DCMAKE_BUILD_TYPE=Release
call msbuild OpenCV.sln /p:Configuration=Debug /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Debug /m:4 
call msbuild OpenCV.sln /p:Configuration=Release /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Release /m:4
echo "%msgprefix% Script finished"
goto :eof
