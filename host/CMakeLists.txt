cmake_minimum_required(VERSION 3.1)

project(CMPT433_Project)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)



# Define the executable
# add_executable(rece rece.cpp)

# What folders to build
# add_subdirectory(hal)  
add_subdirectory(app)

# Link the OpenCV libraries
# target_link_libraries(project_host ${OpenCV_LIBS})

