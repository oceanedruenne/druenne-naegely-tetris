cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TETRIS)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Defining the sources
set(SOURCES
    src/main.c
    src/tetromino.c
    src/tetromino.h
    src/tetris_logic.c
    src/tetris_logic.h
)

# Include directories
include_directories(src)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Set the output directory for the executable
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
