@echo off
setlocal

set project_name=VTK
set file_name=VTK-7.0.0
set build_root=%cd%
set "build_root=%build_root:\=/%"
set extern_root=%build_root%
set export_root=%build_root%/install
set bits=64
set build_configuration=Release
set additional_cmake_options=
set cmake_options=-DVTK_RENDERING_BACKEND=OpenGL -DBUILD_TESTING=0 -DBUILD_EXAMPLES=0

set msgprefix=*** install_%project_name%.bat: 

if exist "%build_root%/%file_name%/" goto install

:unpack
echo %msgprefix% Extracting...
call git clone https://github.com/Kitware/VTK.git VTK-7.0.0
if errorlevel 1 (
  rmdir %file_name% /s /q
  goto :eof 
)
cd VTK-7.0.0
call git checkout v7.0.0
if errorlevel 1 (
  rmdir %file_name% /s /q
  goto :eof 
)

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
call msbuild VTK.sln /p:Configuration=Debug /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Debug /m:4
call msbuild VTK.sln /p:Configuration=Release /m:4
call msbuild INSTALL.vcxproj /p:Configuration=Release /m:4
echo "%msgprefix% Script finished"
goto :eof
