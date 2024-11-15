# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")
add_executable(project_target ${MY_SOURCES})

# Set the path to the custom OpenCV build
set(OpenCV_DIR "../opencv_target/build")

# Find OpenCV in the specified directory
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Threads REQUIRED)

# Make use of the HAL library
target_link_libraries(project_target PRIVATE hal Threads::Threads ${OpenCV_LIBS})

#target_link_libraries(project_target PRIVATE Threads::Threads ${OpenCV_LIBS})

# Copy executable to final location (change `project_target` to project name as needed)
add_custom_command(TARGET project_target POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:project_target>"
     "~/cmpt433/public/myApps/project_target" 
  COMMENT "Copying ARM executable to public NFS directory")

