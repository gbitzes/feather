cmake_minimum_required(VERSION 2.6)

# Fetch all cpp files in all files and folders
file(GLOB_RECURSE sources "*.cpp")
add_library(feather ${sources} )

# Create libraries into ./lib
set_target_properties(feather PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib 
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )

# Installation of headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/feather DESTINATION include)

# Installation of the library
install(TARGETS feather
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

