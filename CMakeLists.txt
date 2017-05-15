cmake_minimum_required(VERSION 2.8)
project(vsal)

# Includes
include(GenerateExportHeader)

# Set version
set(VSAL_MAJOR_VERSION 1)
set(VSAL_MINOR_VERSION 1)
set(VSAL_VERSION ${VSAL_MAJOR_VERSION}.${VSAL_MINOR_VERSION})

# Global configurations
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
#set(CMAKE_DEBUG_POSTFIX "_d")

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DVSAL_STATIC_DEFINE")
else()
	 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Optional 3rd party components
# ===================================================
option(WITH_UEYE "Include IDS uEye support" ON)

# VSAL build components
# ===================================================
option(BUILD_VSAL_PLAYER "Build vsal player application" ON)
option(BUILD_DOCS "Build documentation using Doxygen" ON)
option(BUILD_INTERFACE_CSHARP "Build interface for C#" ON)

# Find dependencies
# ===================================================
# Boost
option(WITH_BOOST_STATIC "Boost static libraries" ON)
set(Boost_USE_STATIC_LIBS ${WITH_BOOST_STATIC})
set(BOOST_ALL_DYN_LINK NOT ${WITH_BOOST_STATIC})
if(WIN32)
	if(${WITH_BOOST_STATIC})
		add_definitions(-DBOOST_ALL_NO_LIB)
	else()
		add_definitions(-DBOOST_ALL_DYN_LINK)
	endif()
endif()
find_package(Boost REQUIRED filesystem regex program_options)

# OpenCV
find_package( OpenCV REQUIRED highgui imgproc imgcodecs video videoio)
include_directories(${OpenCV_INCLUDE_DIRS})

# uEyeSDK
if(WITH_UEYE)
	find_package(uEyeSDK)
endif()

# Docs
if(BUILD_DOCS)
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(STATUS "The documentation won't be build.")
		set(BUILD_DOCS OFF CACHE BOOL "Build documentation using Doxygen" FORCE )
	endif()
endif()

# Interfaces
if(BUILD_INTERFACE_CSHARP)
	find_package(SWIG)
	if(NOT SWIG_FOUND)
		message(STATUS "C# interface won't be build.")
		set(BUILD_INTERFACE_CSHARP OFF CACHE BOOL "Build interface for C#" FORCE )	
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

if(BUILD_VSAL_PLAYER)
	add_subdirectory(vsal_player)
	set(VSAL_TARGETS ${VSAL_TARGETS} vsal_player)
endif()

if(BUILD_DOCS)
	add_subdirectory(doc)
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
configure_file(cmake/vsal-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/vsal-config-version.cmake" @ONLY)
   
# Install the vsal-config.cmake and vsal-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/vsal-config.cmake"
  "${PROJECT_BINARY_DIR}/vsal-config-version.cmake"
  DESTINATION "cmake" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT vsal-targets DESTINATION cmake COMPONENT dev)
