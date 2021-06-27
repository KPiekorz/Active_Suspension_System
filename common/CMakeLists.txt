include_directories(${automatic_suspension_system_SOURCE_DIR}/gui
                    ${automatic_suspension_system_SOURCE_DIR}/control
                    ${automatic_suspension_system_SOURCE_DIR}/model_simulation
                    ${automatic_suspension_system_SOURCE_DIR}/sensors)
                    
add_library(common system_utility system_process)