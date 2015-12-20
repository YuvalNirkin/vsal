# - Find UEye SDK
# Find the native UEye headers and libraries.
#
#  UEYE_SDK_INCLUDE_DIRS - where to find uEye.h, etc.
#  UEYE_SDK_LIBRARIES    - List of libraries when using UEye.
#  UEYE_SDK_FOUND        - True if UEye found.


# Include dir
find_path(UEYE_SDK_INCLUDE_DIR uEye.h
	PATHS 
	$ENV{UEYE_SDK_ROOT}/Develop/include
	$ENV{UEYE_SDK_ROOT}/include
	)

# Finally the library itself

# Determine architecture
if(CMAKE_SIZEOF_VOID_P MATCHES "8")
	find_library(UEYE_SDK_LIB NAMES uEye_api_64 HINTS $ENV{UEYE_SDK_ROOT}/Develop/Lib $ENV{UEYE_SDK_ROOT}/Lib)
else()
	find_library(UEYE_SDK_LIB NAMES uEye_api HINTS $ENV{UEYE_SDK_ROOT}/Develop/Lib $ENV{UEYE_SDK_ROOT}/Lib)
endif()

set(UEYE_SDK_LIBRARIES ${UEYE_SDK_LIB} )
set(UEYE_SDK_INCLUDE_DIRS ${UEYE_SDK_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(uEyeSDK  FOUND_VAR uEyeSDK_FOUND
                                  REQUIRED_VARS UEYE_SDK_LIB UEYE_SDK_INCLUDE_DIR
								  FAIL_MESSAGE "Unable to find uEye SDK! Please set UEYE_SDK_ROOT environment variable to the uEye SDK installation directory.")


mark_as_advanced(UEYE_SDK_INCLUDE_DIR UEYE_SDK_LIB )