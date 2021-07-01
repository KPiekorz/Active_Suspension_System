#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdint.h>

typedef enum
{
    gui_message_control_signal,
    gui_message_model_output,

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
 * @brief  Sand data to pythno gui via UDP socket
 * @note
 * @param  data: pointer to data to send (best will be array)
 * @param  data_len: data lenght in bytes
 * @retval None
 */
void Gui_SendMessage(gui_message_type_t message_type, void * data, uint16_t data_len);

#endif /* GUI_H */
