cmake_minimum_required(VERSION 3.0.0)
project(automatic_suspension_system VERSION 0.1.0)

add_subdirectory(lib)

add_executable(automatic_suspension_system main.c)

target_link_libraries(automatic_suspension_system PUBLIC lib pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_include_directories(automatic_suspension_system PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/lib")