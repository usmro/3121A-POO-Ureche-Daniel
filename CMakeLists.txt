cmake_minimum_required(VERSION 3.10)
project(TrafficSimulation VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
include_directories(include)

# Executable
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(TrafficSimApp ${SRC_FILES})

# Threads (necesar pentru engine-ul de simulare real-time)
find_package(Threads REQUIRED)
target_link_libraries(TrafficSimApp PRIVATE Threads::Threads)

# Basic simple Tests Executable
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
file(GLOB_RECURSE SRC_NO_MAIN src/*.cpp)
list(REMOVE_ITEM SRC_NO_MAIN "${CMAKE_SOURCE_DIR}/src/main.cpp")

if(TEST_FILES)
    add_executable(TrafficSimTests ${TEST_FILES} ${SRC_NO_MAIN})
    target_link_libraries(TrafficSimTests PRIVATE Threads::Threads)
endif()
