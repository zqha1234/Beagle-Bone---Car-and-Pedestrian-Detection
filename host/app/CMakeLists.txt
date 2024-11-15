# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")
add_executable(project_host ${MY_SOURCES})

# Set the path to the custom OpenCV build
set(OpenCV_DIR "../opencv_host/opencv_build")

# Find OpenCV in the specified directory
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Threads REQUIRED)

# Make use of the HAL library
# target_link_libraries(project_host PRIVATE hal Threads::Threads ${OpenCV_LIBS})

target_link_libraries(project_host PRIVATE Threads::Threads ${OpenCV_LIBS})

