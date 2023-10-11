cmake_minimum_required(VERSION 3.23)
project(TrafficJelly)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

#file(GLOB_RECURSE SOURCES "src/*.cpp")
set(SOURCES
        src/main.cpp
        src/action.cpp
        src/action.cpp
        src/node/node.cpp
        src/car.cpp
        src/traffic_model.cpp
        src/edge/edge.cpp
        src/edge/basic_road/basic_road.cpp
        src/edge/basic_road/basic_road_dynamics.cpp
        src/edge/basic_road/basic_road_observation.cpp)

# Define the executable and its source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
include_directories(include)