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

# do some checks if we are on Windows or Linux,
# if so then add some definitions, so we can check within the code
if (WIN32 OR MSVC)
    add_definitions( -DSYSTEM_WINDOWS )
elseif( UNIX )
    add_definitions( -DSYSTEM_LINUX )

    # Use the package PkgConfig to detect GTK+ headers/library files
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

    # Setup CMake to use GTK+, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    include_directories(${GTK2_INCLUDE_DIRS})
    link_directories(${GTK2_LIBRARY_DIRS})

    # Add other flags to the compiler
    add_definitions(${GTK2_CFLAGS_OTHER})
    list(APPEND PROJECT_LIBRARIES ${GTK2_LIBRARIES})
endif( UNIX )

set( SRC_LIST fileHelpers/DICOMLoaderVTK.cpp
              fileHelpers/FileDialog.cpp
              SMfMIAInteractorStyle.cpp
              SMfMIAImageViewer.cpp
              Denoising.cpp
              ActiveContour.cpp
              ContourFromMask.cpp
              Helpers/Converter.cpp
              Helpers/Validator.cpp
              ContourToMask.cpp
              BiasCorrection.cpp
              LevelSetSeg.cpp )

set( HDR_LIST fileHelpers/DICOMLoaderVTK.h
              fileHelpers/FileDialog.h
              SMfMIAInteractorStyle.h
              SMfMIAImageViewer.h
              ColorTable.h
              Denoising.h
              ActiveContour.h
              Helpers/Converter.h
              Helpers/TypeDefinitions.h
              Helpers/Validator.h
              ContourFromMask.h
              ContourToMask.h
              BiasCorrection.h
              LevelSetSeg.h )

# create the executable
add_executable(${PROJECT_NAME} main.cpp ${SRC_LIST} ${HDR_LIST})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})

add_executable( activeContoursTest test/activeContoursTest.cpp ActiveContour.cpp ActiveContour.h )
target_link_libraries( activeContoursTest ${PROJECT_LIBRARIES} )

add_executable( contourFromMaskTest test/contourFromMaskTest.cpp
                                    fileHelpers/DICOMLoaderVTK.cpp
                                    fileHelpers/DICOMLoaderVTK.h
                                    ContourFromMask.cpp
                                    ContourFromMask.h
                                    SMfMIAInteractorStyle.cpp
                                    SMfMIAInteractorStyle.h )
target_link_libraries( contourFromMaskTest ${PROJECT_LIBRARIES} )

add_executable( fit2DMaskContourToImageTest test/fit2DMaskContourToImageTest.cpp ${SRC_LIST} ${HDR_LIST} )
target_link_libraries( fit2DMaskContourToImageTest ${PROJECT_LIBRARIES} )

add_executable( fit3DMaskContourToImageTest test/fit3DMaskContourToImageTest.cpp ${SRC_LIST} ${HDR_LIST} )
target_link_libraries( fit3DMaskContourToImageTest ${PROJECT_LIBRARIES} )

add_executable(levelSetTest test/levelSetTest.cpp ${SRC_LIST} ${HDR_LIST} )
target_link_libraries( levelSetTest ${PROJECT_LIBRARIES} )

add_executable(levelSetCopyPasteTest test/levelSetCopyPasteTest.cpp 
									SMfMIAInteractorStyle.cpp
                                    SMfMIAInteractorStyle.h
									SMfMIAImageViewer.cpp
									SMfMIAImageViewer.h
									 Helpers/Converter.cpp
									 Helpers/Converter.h)
target_link_libraries( levelSetCopyPasteTest ${PROJECT_LIBRARIES} )

