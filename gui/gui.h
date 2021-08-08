#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdint.h>

#define INIT_GUI

// #define INCLUDE_PYTHON_GUI

#define GUI_MODEL_SIMULATION_DATA_SIZE      (8)

typedef enum
{
    gui_message_unknown = 0,
    gui_message_control_signal,
    gui_message_model_simulation_data,

} gui_message_type_t;

/**
 * @brief  Init gui process
 * @note
 * @retval None
 */
void Gui_Init(void);

/**
 * @brief  Destroy gui process
 * @note
 * @retval None
 */
void Gui_Destroy(void);

/**
 * @brief  Sand message to gui process, form any other procerss from the system (pipe).
 *         It will give other process to communicate with each other win convenient way.
 *         (it is only used to communicate with oher process among system, not with other
 *          threads in one process)
 * @note
 * @param  data: pointer to data to send (best will be array)
 * @param  data_len: data lenght in bytes
 * @retval None
 */
void Gui_SendMessage(gui_message_type_t message_type, float * data, int data_len);

#endif /* GUI_H */
