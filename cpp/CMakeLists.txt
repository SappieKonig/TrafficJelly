cmake_minimum_required(VERSION 3.23)
project(TrafficJelly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(SOURCES
        src/action.cpp
        src/action.cpp
        src/node/node.cpp
        src/node/basic_city.cpp
        src/car.cpp
        src/traffic_model.cpp
        src/edge/edge.cpp
        src/edge/basic_road/basic_road.cpp
        src/edge/basic_road/basic_road_dynamics.cpp
        src/edge/basic_road/basic_road_observation.cpp
        src/route.cpp)

enable_testing()

add_executable(load_test tests/load_test.cpp ${SOURCES})
target_include_directories(load_test PRIVATE include)
add_test(load_test load_test)

add_executable(save_test tests/save_test.cpp ${SOURCES})
target_include_directories(save_test PRIVATE include)
add_test(save_test save_test)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
