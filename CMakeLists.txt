cmake_minimum_required(VERSION 3.20)
# TODO Add variable for base devkit location
# TODO Add post build commands for moving files

project(blendShapeDeltas
    VERSION 1.0
    DESCRIPTION "Updates Models based on deltas to between old and new"
    LANGUAGES CXX)

if(DEFINED ENV{MAYAVERSION})
else()
    set (ENV{MAYAVERSION} 2024)
endif()


# NOTE these can also be set via external calling shell script
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(DEFINED ENV{DEVKIT_LOCATION})
    else()
    set (ENV{DEVKIT_LOCATION}/Users/matthewschiller/sb/MayaDevkits$ENV{MAYAVERSION})
    endif()
    
    if(DEFINED ENV{MAYA_LOCATION})
    else()
        set(ENV{MAYA_LOCATION}/Applications/Autodesk/maya${MAYAVERSION}/Maya.app/Contents)
    endif()
    set(ENV{DYLD_LIBRARY_PATH} $ENV{MAYA_LOCATION}/MacOS)
    set(ENV{DYLD_LIBRARY_PATH} $ENV{MAYA_LOCATION}/Frameworks)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(DEFINED ENV{DEVKIT_LOCATION})
    else()
        set(ENV{DEVKIT_LOCATION} "Z:/devkits/$ENV{MAYAVERSION}/devkitBase")
    endif()
    
    if(DEFINED ENV{MAYA_LOCATION})
    else()
        set(ENV{MAYA_LOCATION} "C:/Program Files/Autodesk/Maya$ENV{MAYAVERSION}/bin")
    endif()
endif()

message(MAYAVERSION="$ENV{MAYAVERSION}")
message(MAYA_LOCATION="$ENV{MAYA_LOCATION}")
message(DEVKIT_LOCATION="$ENV{DEVKIT_LOCATION}")

# Sets up the maya cmake
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME blendShapeDeltas)

set(CMAKE_AUTOMOC TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(PROJECT_MAIN blendShapeDeltas)

add_compile_definitions("NDEBUG")

set(LIBRARIES
    OpenMaya
    OpenMayaAnim
    OpenMayaFX
    OpenMayaRender
    OpenMayaUI
    Foundation
)

set(SOURCE_FILES 
    src/blendShapeDeltas.cpp 
    src/blendShapeDeltas.h 
)

build_plugin()