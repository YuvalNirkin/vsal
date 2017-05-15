set(SRC vsal_player.cpp)
if(MSVC)
	set(SRC ${SRC} vsal_player.rc)
endif()
add_executable(vsal_player ${SRC})
target_include_directories(vsal_player PRIVATE 
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(vsal_player PRIVATE 
	vsal
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

install(TARGETS vsal_player EXPORT vsal-targets DESTINATION bin COMPONENT bin)
