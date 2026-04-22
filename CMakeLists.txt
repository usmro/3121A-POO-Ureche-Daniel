cmake_minimum_required(VERSION 3.10)
project(TrafficSimulation VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
include_directories(include)

# Executable
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(TrafficSimApp ${SRC_FILES})

# Basic simple Tests Executable (optional for now, will configure properly in Stage 5, but let's add a placeholder)
# file(GLOB_RECURSE TEST_FILES tests/*.cpp)
# if(TEST_FILES)
#     add_executable(TrafficSimTests ${TEST_FILES})
# endif()
