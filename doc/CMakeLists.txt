#--------------------------
#  CMake file for vsal docs
#--------------------------

# additional config
set(doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
set(CMAKE_DOXYGEN_OUTPUT_PATH "doxygen")

# Write doxygen configuration file
configure_file(Doxyfile.in ${doxyfile} @ONLY)

add_custom_target(doxygen
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
	DEPENDS ${doxyfile}
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html
	DESTINATION doc
	COMPONENT "docs" OPTIONAL
)