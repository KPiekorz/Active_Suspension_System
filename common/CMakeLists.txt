include_directories(${automatic_suspension_system_SOURCE_DIR}/gui
                    ${automatic_suspension_system_SOURCE_DIR}/control
                    ${automatic_suspension_system_SOURCE_DIR}/model_simulation
                    ${automatic_suspension_system_SOURCE_DIR}/sensors)

add_library(signals_handler signals_handler)
add_library(PID PID)
add_library(matrix_lib matrix_lib)
add_library(system_utility system_utility)
add_library(system_process system_process)
target_link_libraries(system_process PUBLIC system_utility)
