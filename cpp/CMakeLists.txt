cmake_minimum_required(VERSION 3.23)
project(traffic_model)

# configure pybind11
list(APPEND CMAKE_PREFIX_PATH "/usr/local/share/cmake/pybind11")
find_package(pybind11 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(SOURCES
        src/action.cpp
        src/action.cpp
        src/node.cpp
        src/basic_city.cpp
        src/car.cpp
        src/traffic_model.cpp
        src/edge.cpp
        src/basic_road.cpp
        src/basic_road_dynamics.cpp
        src/basic_road_observation.cpp
        src/route.cpp
        src/route_planner.cpp
        src/car_parameters.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)

enable_testing()

add_executable(load_test tests/load_test.cpp ${SOURCES})
target_include_directories(load_test PRIVATE include)
add_test(load_test load_test)

add_executable(save_test tests/save_test.cpp ${SOURCES})
target_include_directories(save_test PRIVATE include)
add_test(save_test save_test)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Define the executable and its source files
add_library(${PROJECT_NAME} MODULE ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "traffic_model")
set_target_properties(traffic_model PROPERTIES PREFIX "")
