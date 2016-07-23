# Validation
if(NOT uEyeSDK_FOUND)
	message(STATUS "vsal will be built without IDS uEye support.")
endif()

# Source
file(GLOB VSAL_INCLUDE "include/vsal/*.h")
file(GLOB VSAL_SRC "src/*.h" "src/*.cpp")
list(APPEND VSAL_SRC ${VSAL_INCLUDE})
if(uEyeSDK_FOUND)
	file(GLOB VSAL_UEYE_SRC "src/uEye/*.h" "src/uEye/*.cpp")
	list(APPEND VSAL_SRC ${VSAL_UEYE_SRC})
	add_definitions(-DSUPPORT_UEYE)
	include_directories(${UEYE_SDK_INCLUDE_DIRS})
	set(VSAL_OPT_LIBS ${UEYE_SDK_LIBRARIES})
endif()

source_group("include\\vsal" FILES ${VSAL_INCLUDE})
#message(STATUS "VSAL_SRC = " ${VSAL_SRC})

include_directories(include)
add_library(vsal SHARED ${VSAL_SRC})
target_link_libraries(vsal PRIVATE opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio ${VSAL_OPT_LIBS})
if(UNIX)
	target_link_libraries(vsal PRIVATE ${Boost_LIBRARIES})
endif(UNIX)
set_target_properties(vsal PROPERTIES DEBUG_POSTFIX _d)

# Add export header
if(UNIX)
	set(USE_COMPILER_HIDDEN_VISIBILITY OFF)
endif(UNIX)
generate_export_header(vsal EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/vsal/vsal_export.h)
install(FILES ${PROJECT_BINARY_DIR}/include/vsal/vsal_export.h DESTINATION include/vsal)

# Installations
install(TARGETS vsal
	 EXPORT vsal-targets
     RUNTIME DESTINATION bin COMPONENT dev
     LIBRARY DESTINATION lib COMPONENT dev
     ARCHIVE DESTINATION lib COMPONENT dev)
#install(TARGETS vsal DESTINATION lib)
install(FILES ${VSAL_INCLUDE} DESTINATION include/vsal)
