file(GLOB VSAL_INCLUDE "include/vsal/*.h")
file(GLOB VSAL_SRC "src/*.h" "src/*.cpp")
list(APPEND VSAL_SRC ${VSAL_INCLUDE})
if(WITH_UEYE)
	file(GLOB VSAL_UEYE_SRC "src/uEye/*.h" "src/uEye/*.cpp")
	list(APPEND VSAL_SRC ${VSAL_UEYE_SRC})
	add_definitions(-DSUPPORT_UEYE)
endif()

source_group("include\\vsal" FILES ${VSAL_INCLUDE})
#message(STATUS "VSAL_SRC = " ${VSAL_SRC})

include_directories(include)
add_library(vsal SHARED ${VSAL_SRC})
target_link_libraries(vsal PRIVATE opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio)

# Add export header
generate_export_header(vsal)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/vsal)
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/vsal_export.h ${CMAKE_CURRENT_BINARY_DIR}/vsal/vsal_export.h)
target_include_directories(vsal PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${PROJECT_BINARY_DIR}/vsal/vsal_export.h DESTINATION include/vsal)

install(TARGETS vsal DESTINATION lib)
install(FILES ${VSAL_INCLUDE} DESTINATION include/vsal)