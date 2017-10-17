@echo off
setlocal

set project_name=liverSegmenter
set file_name=liverSegmenter
set src_root=%cd%
set "src_root=%src_root:\=/%"
set build_root=%src_root%/build
set extern_root=%src_root%/3rd-party
set export_root=%extern_root%/install
set bits=64
set build_configuration=Release
set additional_cmake_options=
set cmake_options=-DITK_DIR=%export_root%/lib/cmake/ITK-4.12 -DVTK_DIR=%export_root%/lib/cmake/vtk-7.0 -DOPENCV_DIR=%export_root%/share/OpenCV

echo "==== VTK_DIR = %export_root%/lib/cmake/vtk-7.0 ==="

set msgprefix=*** install_%project_name%.bat:

if exist "%export_root%" goto build

:3rd-party
pushd 3rd-party
call install_opencv.bat
call install_vtk.bat
call install_itk.bat
popd

:build
if not exist "%build_root%" mkdir "%build_root%"

pushd %build_root%
:: set all system variables, so we find visual studio
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build" (
set compiler="Visual Studio 15 2017 Win64"
set variableScript="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
) 

cd %build_root%
call %variableScript% amd64

call cmake -G %compiler% %src_root%/src %cmake_options% %additional_cmake_options% -DCMAKE_INSTALL_PREFIX="%export_root%" -DCMAKE_BUILD_TYPE=%build_configuration%
call msbuild liverSegmenter.sln /p:Configuration=%build_configuration% /m:4

echo "%msgprefix% Script finished"

popd
goto :eof
