#! /bin/sh

build_root="$(pwd)"
extern_root="${build_root}"
export_root="${build_root}/install"
architecture="x64_linux"
build_configuration="Release"
additional_cmake_options=""
version=v4.12.2
# disable everything except ? and ?
itk_cmake_options="-DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DModule_ITKVtkGlue=ON -DVTK_DIR=${export_root}/lib/cmake/vtk-7.0 -DModule_ITKVideoBridgeOpenCV=ON -DOPENCV_DIR=${export_root}/share/OpenCV"

## function to print error message, then exit the script
error()
{
  echo "*** install_itk.sh: $1"
  exit 1
}

## function to print status message or warnung
## the script uses this function instead of echo for console printing
message()
{
  echo "*** install_itk.sh: $1"
}

message "ITK install script"

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

if [ ! -e ${extern_root}/ITK-$version ]
then
  message "extracting"
  cd ${extern_root}
  ( git clone https://github.com/InsightSoftwareConsortium/ITK.git ITK-$version ) || error "failed to extract ITK archive"
  cd ITK-$version
  git checkout $version
  cd ${extern_root}
fi

mkdir -p "${export_root}/include"
mkdir -p "${export_root}/lib"
mkdir -p "${extern_root}/ITK-$version/build_${arch_path}"

cd "${extern_root}/ITK-$version/build_${arch_path}"

message "building"
( cmake .. ${itk_cmake_options} ${additional_cmake_options} "-DCMAKE_INSTALL_PREFIX=${export_root}" -DCMAKE_BUILD_TYPE=${build_configuration} ) || error "failed cmake"
( make -j4 install ) || error "failed make"

cd ..
#rm -rf "build_${arch_path}"
