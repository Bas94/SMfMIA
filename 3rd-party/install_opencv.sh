#! /bin/sh

build_root="$(pwd)"
extern_root="${build_root}"
export_root="${build_root}/install"
architecture="x64_linux"
build_configuration="Release"
additional_cmake_options=""
version="3.1.0"
library_name="opencv-$version"
library_name_contrib="opencv_contrib-$version"
# disable everything except ? and ?
cmake_options="-DBUILD_DOCS=0 -DBUILD_EXAMPLES=0 -DBUILD_FAT_JAVA_LIB=0 -DBUILD_JPEG=0 -DWITH_OPENEXR=0 -DBUILD_PACKAGE=1 -DBUILD_PERF_TESTS=0 -DBUILD_PNG=0 -DBUILD_SHARED_LIBS=0 -DBUILD_TBB=0 -DWITH_IPP=0 -DBUILD_TESTS=0 -DBUILD_WITH_DEBUG_INFO=1 -DBUILD_ZLIB=0 -DBUILD_apps=0 -DBUILD_calib3d=1 -DBUILD_contrib=0 -DBUILD_legacy=0 -DBUILD_core=1 -DBUILD_features2d=1 -DBUILD_photo=1 -DWITH_CUDA=0 -DWITH_FFMPEG=1 -DFFMPEG_INCLUDE_DIR='/usr/include/ffmpeg' -DWITH_LIB4L=0-DWITH_JPEG=1 -DWITH_JASPER=0 -DWITH_PNG=1 -DWITH_PVAPI=0 -DWITH_NVCUVID=0 -DWITH_OPENCL=0 -DWITH_OPENCLAMDBLAS=0 -DWITH_OPENCLAMDFFT=0 -DWITH_TIFF=0 -DWITH_LIBV4L=0 -DWITH_V4L=0 -DWITH_VTK=0 -DWITH_WEBP=0 -DBUILD_ml=1 -DBUILD_flann=1 -DBUILD_video=1 -DBUILD_videoio=1 -DBUILD_videostab=0 -DBUILD_gpu=0 -DBUILD_python2=0 -DBUILD_stitching=1 -DBUILD_ts=0 -DOPENCV_EXTRA_MODULES_PATH=${extern_root}/${library_name_contrib}/modules -DBUILD_opencv_bioinspired=0 -DBUILD_opencv_xfeatures2d=1 -DBUILD_opencv_ximgproc=1 -DBUILD_opencv_xobjdetect=1 -DBUILD_opencv_xphoto=1 -DCMAKE_CXX_FLAGS=-std=gnu++11"
script_name=`basename "$0"`

## function to print error message, then exit the script
error()
{
  echo "*** ${script_name}: $1"
  exit 1
}

## function to print status message or warnung
## the script uses this function instead of echo for console printing
message()
{
  echo "*** ${script_name}: $1"
}

message "${script_name} script"

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

if [ ! -e ${extern_root}/${library_name} ]
then
  message "extracting"
  cd ${extern_root}
  ( git clone https://github.com/opencv/opencv.git ${library_name} ) || error "failed to clone ${library_name} git archive"
  cd ${library_name}
  ( git checkout $version ) || error "failed to checkout version tag git archive"
  cd ${extern_root}
fi

if [ ! -e ${extern_root}/${library_name_contrib} ]
then
  message "extracting"
  cd ${extern_root}
  ( git clone https://github.com/opencv/opencv_contrib.git ${library_name_contrib} ) || error "failed to clone ${library_name_contrib} git archive"
  cd ${library_name_contrib}
  ( git checkout $version ) || error "failed to checkout version tag git archive"
  cd ${extern_root}
fi


mkdir -p "${export_root}/include"
mkdir -p "${export_root}/lib"
mkdir -p "${extern_root}/${library_name}/build_${arch_path}"

cd "${extern_root}/${library_name}/build_${arch_path}"

message "building"
( cmake .. ${cmake_options} ${additional_cmake_options} "-DCMAKE_INSTALL_PREFIX=${export_root}" -DCMAKE_BUILD_TYPE=${build_configuration} ) || error "failed cmake"
( make -j4 install ) || error "failed make"

cd ..
#rm -rf "build_${arch_path}"
