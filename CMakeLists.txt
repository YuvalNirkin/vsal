cmake_minimum_required(VERSION 2.8)
project( vsal )

include(GenerateExportHeader)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Optional 3rd party components
# ===================================================
option(WITH_UEYE "Include IDS uEye support" ON)

# VSAL build components
# ===================================================
option(BUILD_PREVIEW "Build preview application" ON)

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

find_package( OpenCV REQUIRED highgui imgproc imgcodecs video videoio)

if(WITH_UEYE)
	find_package(UEyeSDK)
	if(NOT UEYE_SDK_FOUND)
		message(FATAL_ERROR "Couldn't find uEye SDK! To build without uEye support set WITH_UEYE to OFF")
	endif()
endif()
#message(STATUS "OpenCV_LIBS = " ${OpenCV_LIBS})

# Set build output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------------------------
# Process subdirectories
# ----------------------------------------------------------------------------
add_subdirectory(vsal)

if(BUILD_PREVIEW)
	add_subdirectory(preview)
endif()
