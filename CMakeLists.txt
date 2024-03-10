cmake_minimum_required(VERSION 3.18)

# create the project
project(main)

# when using debuggers such as gdb, the following line can be used
#set(CMAKE_BUILD_TYPE Debug)

# if you did not build RadioLib as shared library (see wiki),
# you will have to add it as source directory
# the following is just an example, yours will likely be different
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../lib/RadioLib" "${CMAKE_CURRENT_BINARY_DIR}/RadioLib")

# add the executable
add_executable(${PROJECT_NAME} main.cpp)
include_directories(/usr/local/include)

# include the WiringPi headers
include_directories(${WIRINGPI_INCLUDE_DIRS})

# link both libraries
target_link_libraries(${PROJECT_NAME} RadioLib pigpio  /usr/local/lib/libwiringPi.so)

# you can also specify RadioLib compile-time flags here
#target_compile_definitions(${PROJECT_NAME} PUBLIC RADIOLIB_DEBUG RADIOLIB_VERBOSE)
#target_compile_definitions(${PROJECT_NAME} PUBLIC RADIOLIB_DEBUG RADIOLIB_VERBOSE)