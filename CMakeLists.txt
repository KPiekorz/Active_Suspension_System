cmake_minimum_required(VERSION 3.0.0)
project(automatic_suspension_system VERSION 0.0.0)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(common)
add_subdirectory(control)
add_subdirectory(gui)
add_subdirectory(model_simulation)
add_subdirectory(sensors)

add_executable(automatic_suspension_system main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_include_directories(automatic_suspension_system PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/common"
                          "${PROJECT_SOURCE_DIR}/sensors"
                          "${PROJECT_SOURCE_DIR}/control"
                          "${PROJECT_SOURCE_DIR}/model_simulation"
                          "${PROJECT_SOURCE_DIR}/gui")

target_link_libraries(automatic_suspension_system PUBLIC  common sensors gui model_simulation control ${PYTHON_LIBRARIES})
