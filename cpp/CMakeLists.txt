cmake_minimum_required(VERSION 3.23)
project(traffic_model)

# configure pybind11
list(APPEND CMAKE_PREFIX_PATH "/usr/local/share/cmake/pybind11")
find_package(pybind11 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


file(GLOB_RECURSE SOURCES "src/*.cpp")

# Define the executable and its source files
add_library(${PROJECT_NAME} MODULE ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "traffic_model")
set_target_properties(traffic_model PROPERTIES PREFIX "")


# Include directories
include_directories(include)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module)
