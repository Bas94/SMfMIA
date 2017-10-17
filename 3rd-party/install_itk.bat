@echo off
setlocal

set project_name=ITK
set version=v4.12.2
set file_name=ITK-%version%
set build_root=%cd%
set "build_root=%build_root:\=/%"
set extern_root=%build_root%
set export_root=%build_root%/install
set bits=64
set build_configuration=Release
set additional_cmake_options=
set cmake_options=-DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DModule_ITKVtkGlue=ON -DVTK_DIR=%export_root%/lib/cmake/vtk-7.0 -DModule_ITKVideoBridgeOpenCV=ON -DOPENCV_DIR=%export_root%/share/OpenCV

set msgprefix=*** install_%project_name%.bat: 

if exist "%build_root%/%file_name%/" goto install

:unpack
echo %msgprefix% Extracting...
call git clone https://github.com/Kitware/ITK.git ITK-%version%
if errorlevel 1 (
  rmdir %file_name% /s /q
  goto :eof 
)
cd ITK-%version%
call git checkout %version%
if errorlevel 1 (
  rmdir %file_name% /s /q
  goto :eof 
)

:install
if not exist "%export_root%/include" mkdir "%export_root%/include"
if not exist "%export_root%/lib" mkdir "%export_root%/lib"
if not exist "%export_root%/%file_name%/build_%bits%" mkdir "%export_root%/%file_name%/build_%bits%"
if not exist "%build_root%/../buildITK" mkdir "%build_root%/../buildITK"
mklink /j "%export_root%/%file_name%/build_%bits%" "%build_root%/../buildITK"
pushd "%build_root%/../buildITK"
set shortBuildDir=%cd%
echo "=== using short build path: %shortBuildDir% ==="
popd
cd "%shortBuildDir%"

:: set all system variables, so we find visual studio
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build" (
set compiler="Visual Studio 15 2017 Win64"
set variableScript="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
) 

call %variableScript% amd64

call cmake -G %compiler% %build_root%/%file_name% %cmake_options% %additional_cmake_options% -DCMAKE_INSTALL_PREFIX="%export_root%" -DCMAKE_BUILD_TYPE=Release
call msbuild ITK.sln /p:Configuration=Debug /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Debug /m:4
call msbuild ITK.sln /p:Configuration=Release /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Release /m:4

:: rmdir "%shortBuildDir%"
echo "%msgprefix% Script finished"
goto :eof
