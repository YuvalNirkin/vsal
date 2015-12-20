file(GLOB VSAL_INCLUDE "include/vsal/*.h")
file(GLOB VSAL_SRC "src/*.h" "src/*.cpp")
list(APPEND VSAL_SRC ${VSAL_INCLUDE})
if(WITH_UEYE)
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

# Add export header
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