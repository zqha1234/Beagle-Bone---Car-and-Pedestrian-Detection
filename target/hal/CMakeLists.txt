# CMakeList.txt for HAL
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"



# Include header files
include_directories(hal/include)

# Collect all source files
file(GLOB MY_SOURCES "src/*.cpp")

# Create a static library 'hal' from the sourced files
add_library(hal STATIC ${MY_SOURCES})

# Specify where the public includes for this library are located
target_include_directories(hal PUBLIC include)

# Set the path to the custom OpenCV build
set(OpenCV_DIR "../opencv_target/build")

# Find OpenCV in the specified directory
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("../opencv_target/include")
include_directories("../opencv_target/build")
include_directories("../opencv_target/modules/core/include")


# Link OpenCV libraries to 'hal'
# Ensure OpenCV has been found via find_package(OpenCV REQUIRED) before this line
target_link_libraries(hal PUBLIC ${OpenCV_LIBS})

