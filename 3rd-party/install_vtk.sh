#! /bin/sh

build_root="$(pwd)"
extern_root="${build_root}"
export_root="${build_root}/install"
architecture="x64_linux"
build_configuration="Release"
additional_cmake_options=""
# disable everything except ? and ?
vtk_cmake_options="-DVTK_RENDERING_BACKEND=OpenGL2 -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_CXX_FLAGS=-std=gnu++11"

## function to print error message, then exit the script
error()
{
  echo "*** install_vtk.sh: $1"
  exit 1
}

## function to print status message or warnung
## the script uses this function instead of echo for console printing
message()
{
  echo "*** install_vtk.sh: $1"
}

message "VTK install script"

if [ $# -ge 1 ]
then
  architecture="$1"
fi

case "${architecture}" in
  "x86_linux")
    arch_path="x86_linux"
    ;;
  "x64_linux")
    arch_path="x64_linux"
    ;;
  *)
    error "invalid architecture, expected x86_linux, x64_linux"
    ;;
esac

if [ $# -ge 2 ]
then
  build_configuration="$2"
fi

case "${build_configuration}" in
  "Release")
    ;;
  "Debug")
    ;;
  *)
    error "invalid configuration, expected Release, Debug"
    ;;
esac

if [ ! -e ${extern_root}/VTK-7.0.0 ]
then
  message "extracting"
  cd ${extern_root}
  ( git clone https://github.com/Kitware/VTK.git VTK-7.0.0 ) || error "failed to extract VTK archive"
  cd VTK-7.0.0
  git checkout 7.0.0
  cd ${extern_root}
fi

mkdir -p "${export_root}/include"
mkdir -p "${export_root}/lib"
mkdir -p "${extern_root}/VTK-7.0.0/build_${arch_path}"

cd "${extern_root}/VTK-7.0.0/build_${arch_path}"

message "building"
( cmake .. ${vtk_cmake_options} ${additional_cmake_options} "-DCMAKE_INSTALL_PREFIX=${export_root}" -DCMAKE_BUILD_TYPE=${build_configuration} ) || error "failed cmake"
( make -j4 install ) || error "failed make"

cd ..
#rm -rf "build_${arch_path}"
