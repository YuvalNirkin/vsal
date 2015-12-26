find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/vsal/include)
set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(vsal_csharp.i PROPERTIES CPLUSPLUS ON)
set(CMAKE_SWIG_FLAGS -namespace vsal)
#set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/vsal/interfaces/csharp/gen)
swig_add_module(vsal_csharp csharp vsal_csharp.i)
target_link_libraries(vsal_csharp vsal)
set_target_properties(vsal_csharp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installations
install(TARGETS vsal_csharp
     ARCHIVE DESTINATION lib COMPONENT dev
	 LIBRARY DESTINATION interfaces/csharp/bin COMPONENT dev
	 RUNTIME DESTINATION interfaces/csharp/bin COMPONENT dev)
	 
install(FILES vsal_csharp.i DESTINATION interfaces/csharp)
	 
install(
	CODE "file( REMOVE \"${CMAKE_CURRENT_BINARY_DIR}/vsal_csharp.cs\" )"
    CODE "file( GLOB GeneratedSources \"${CMAKE_CURRENT_BINARY_DIR}/*.cs\" )"
    CODE "file( INSTALL \${GeneratedSources} DESTINATION \"${CMAKE_INSTALL_PREFIX}/interfaces/csharp/src\" )"
)

#install(
 #   CODE "file( GLOB GeneratedSources \"${CMAKE_BINARY_DIR}/interfaces/csharp/*.cs\" )"
 #   CODE "file( INSTALL \${GeneratedSources} DESTINATION \"interfaces/csharp\" )"
#)