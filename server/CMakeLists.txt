cmake_minimum_required(VERSION 2.6)

add_definitions("-std=c++0x -pthread")

# Fetch all cpp files in all files and folders
file(GLOB_RECURSE sources "*.cpp")
add_executable(server ${sources})

# We also need access to the private headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Link
target_link_libraries(server feather pthread)
set_target_properties(server 
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
