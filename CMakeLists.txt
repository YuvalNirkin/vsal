cmake_minimum_required(VERSION 2.8)
project( vsal )

include(GenerateExportHeader)

# Global configurations
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_DEBUG_POSTFIX "_d")

# Optional 3rd party components
# ===================================================
option(WITH_UEYE "Include IDS uEye support" ON)

# VSAL build components
# ===================================================
option(BUILD_PREVIEW "Build preview application" ON)
option(BUILD_INTERFACE_CSHARP "Build interface for c#" ON)

# Find dependencies
# ===================================================
# Boost
set(Boost_USE_STATIC_LIBS ON)	# only find static libs
set(Boost_USE_MULTITHREADED ON)
find_package( Boost REQUIRED filesystem regex program_options)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif()

# OpenCV
find_package( OpenCV REQUIRED highgui imgproc imgcodecs video videoio)
include_directories(${OpenCV_INCLUDE_DIRS})

# uEyeSDK
if(WITH_UEYE)
	find_package(uEyeSDK)
	if(NOT uEyeSDK_FOUND)
		message(FATAL_ERROR "Couldn't find uEye SDK! To build without uEye support set WITH_UEYE to OFF")
	endif()
endif()

# Set build output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------------------------
# Process subdirectories
# ----------------------------------------------------------------------------
include_directories(${PROJECT_BINARY_DIR}/include)
add_subdirectory(vsal)
set(VSAL_TARGETS vsal)

if(BUILD_PREVIEW)
	add_subdirectory(preview)
	set(VSAL_TARGETS ${VSAL_TARGETS} preview)
endif()

# interfaces
if(BUILD_INTERFACE_CSHARP)
	add_subdirectory(interfaces/csharp)
endif()

# Export configuration
# ===================================================

# Add all targets to the build-tree export set
export(TARGETS ${VSAL_TARGETS}
  FILE "${PROJECT_BINARY_DIR}/vsal-targets.cmake")
  
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE vsal)

# Create the vsal-config.cmake and vsal-config-version.cmake files
set(CONF_INCLUDE_DIRS "\${VSAL_CMAKE_DIR}/include")
configure_file(cmake/vsal-config.cmake.in
  "${PROJECT_BINARY_DIR}/vsal-config.cmake" @ONLY)
   
# Install the vsal-config.cmake and vsal-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/vsal-config.cmake"
  DESTINATION "cmake" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT vsal-targets DESTINATION cmake COMPONENT dev)
