cmake_minimum_required(VERSION 2.27.4)
project(TrafficJelly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

enable_testing()

add_executable(load_test
    tests/load_test.cpp
    src/traffic_model.cpp
    src/node.cpp
    src/edge.cpp
    src/car.cpp
    src/route_planner.cpp
    src/route.cpp
    src/action_generator.cpp
    src/action_manager.cpp
    src/action.cpp
)
target_include_directories(load_test PRIVATE include)
add_test(load_test load_test)

add_executable(save_test
    tests/save_test.cpp
    src/traffic_model.cpp
    src/node.cpp
    src/edge.cpp
    src/car.cpp
    src/route_planner.cpp
    src/route.cpp
    src/action_generator.cpp
    src/action_manager.cpp
    src/action.cpp
)
target_include_directories(save_test PRIVATE include)
add_test(save_test save_test)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/traffic_model.cpp
    src/node.cpp
    src/edge.cpp
    src/car.cpp
    src/route_planner.cpp
    src/route.cpp
    src/action_generator.cpp
    src/action_manager.cpp
    src/action.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE include)
