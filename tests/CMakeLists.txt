cmake_minimum_required(VERSION 2.6)

set(GTEST ${CMAKE_SOURCE_DIR}/external/gtest )
set(GTEST_BINARY_DIR ${CMAKE_BINARY_DIR}/external/gtest )

add_definitions("-std=c++0x")

# Download and configure gtest
if (NOT EXISTS ${GTEST} )
	message("Gtest not found - downloading and extracting...")
	set(GTEST_VERSION gtest-1.7.0)
	set(GTEST_FILENAME ${GTEST_VERSION}.zip)
	set(GTEST_URL https://googletest.googlecode.com/files/${GTEST_FILENAME} )

	file(DOWNLOAD ${GTEST_URL} ${CMAKE_BINARY_DIR}/${GTEST_FILENAME} )
	execute_process(COMMAND ${CMAKE_COMMAND} -E 
		tar -xf ${CMAKE_BINARY_DIR}/${GTEST_FILENAME} )
	execute_process(COMMAND ${CMAKE_COMMAND} -E 
		copy_directory ${CMAKE_BINARY_DIR}/${GTEST_VERSION} ${GTEST} )
	message("Done!")
endif()

# Fetch all cpp files in all files and folders
file(GLOB_RECURSE sources "*.cpp")
add_executable(tests ${sources})
include_directories(${GTEST}/include)

# The tests also need access to the private headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Build gtest
add_subdirectory(${GTEST} ${GTEST_BINARY_DIR} )

# Link
target_link_libraries(tests feather gtest_main)
set_target_properties(tests 
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
