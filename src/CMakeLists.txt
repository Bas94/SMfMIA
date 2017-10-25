cmake_minimum_required(VERSION 2.8.10)
project(liverSegmenter)

set( OPENCV_DIR "" CACHE PATH "Path to opencv main directory" )
set( PROJECT_LIBRARIES "")

# set path to all CMAKE_MODULES
set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )
# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON )

# add some compiler flags

# list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Werror" )
# list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall" )

# add VTK to project

#set( VTK_DIR "${SDK_PATH}/bin/vtk-7.0.0/lib/cmake/vtk-7.0/")
find_package(VTK 7.0 REQUIRED)
message( STATUS "VTK_USE_FILE = ${VTK_USE_FILE}" )
include(${VTK_USE_FILE})
list(APPEND PROJECT_LIBRARIES ${VTK_LIBRARIES})
message( STATUS "VTK libraries = ${VTK_LIBRARIES}" )
include_directories(VTK_INCLUDE_DIR)

# add ITK to project

#set( ITK_DIR "${ITK_DIR}/lib/cmake/ITK-4.7/" )
FIND_PACKAGE ( ITK REQUIRED )
include( ${ITK_USE_FILE} )
# add all itk libraries
list( APPEND PROJECT_LIBRARIES
        ITKVideoBridgeOpenCV
        ITKBiasCorrection
        ITKBioCell
        ITKCommon
        ITKDICOMParser
        ITKEXPAT
        ITKFEM
        ITKgiftiio
        ITKIOBioRad
        ITKIOBMP
        ITKIOCSV
        ITKIOGDCM
        ITKIOGE
        ITKIOGIPL
        ITKIOHDF5
        ITKIOImageBase
        ITKIOIPL
        ITKIOJPEG
        ITKIOLSM
        ITKIOMesh
        ITKIOMeta
        ITKIOMRC
        ITKIONIFTI
        ITKIONRRD
        ITKIOPNG
        ITKIOSiemens
        ITKIOSpatialObjects
        ITKIOStimulate
        ITKIOTIFF
        ITKIOTransformBase
        ITKIOTransformHDF5
        ITKIOTransformInsightLegacy
        ITKIOTransformMatlab
        ITKIOVTK
        ITKIOXML
        ITKKLMRegionGrowing
        ITKLabelMap
        ITKMesh
        ITKMetaIO
        ITKniftiio
        ITKNrrdIO
        ITKOptimizers
        ITKOptimizersv4
        ITKPath
        ITKPolynomials
        ITKQuadEdgeMesh
        ITKSpatialObjects
        ITKStatistics
        ITKVideoCore
        ITKVideoIO
        ITKVNLInstantiation
        ITKVTK
        ITKWatersheds
        ITKznz )
message( STATUS ${ITK_USE_FILE} )
# include_directories( "${ITK_INCLUDE_DIRS}" )

# add opencv to project
find_package( OpenCV REQUIRED )
list(APPEND PROJECT_LIBRARIES ${OpenCV_LIBS})
include_directories( "${OPENCV_DIR}/include" )

set( SRC_LIST DICOMLoaderVTK.cpp
              myVTKInteractorStyle.cpp )
set( HDR_LIST DICOMLoaderVTK.h
              myVTKInteractorStyle.h )

# create the executable
add_executable(${PROJECT_NAME} main.cpp ${SRC_LIST} ${HDR_LIST})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})
