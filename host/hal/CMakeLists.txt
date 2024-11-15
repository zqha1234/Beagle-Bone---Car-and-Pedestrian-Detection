# CMakeList.txt for HAL
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"



#include_directories(hal/include)
#file(GLOB MY_SOURCES "src/*.cpp")

#add_library(hal STATIC ${MY_SOURCES})

#target_include_directories(hal PUBLIC include)


# Set the minimum required version of cmake for this project
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(HAL)

# Include header files
include_directories(include)

# Collect all source files
file(GLOB MY_SOURCES "src/*.cpp")

# Create a static library 'hal' from the sourced files
add_library(hal STATIC ${MY_SOURCES})

# Specify where the public includes for this library are located
target_include_directories(hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

