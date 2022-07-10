
#[[
    All that cmake have to be refactored.
]]

cmake_minimum_required(VERSION 3.0.0)
project(automatic_suspension_system VERSION 0.0.0)
project(automatic_suspension_system C)

set(CMAKE_C_FLAGS -pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

include_directories(common)
include_directories(control)
include_directories(gui)
include_directories(model_simulation)
include_directories(sensors)

link_directories(common)
link_directories(control)
link_directories(gui)
link_directories(model_simulation)
link_directories(sensors)

add_subdirectory(common)
add_subdirectory(control)
add_subdirectory(gui)
add_subdirectory(model_simulation)
add_subdirectory(sensors)

add_executable(automatic_suspension_system main.c)

find_library(LIBRT rt)

target_link_libraries(automatic_suspension_system system_process model_simulation control gui sensors matrix_lib PID signals_handler ${LIBRT} ${PYTHON_LIBRARIES})
