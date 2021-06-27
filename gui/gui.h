#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <stdint.h>

/**
 * @brief  Init gui module
 * @note   
 * @retval None
 */
void Gui_Init(void);

/**
 * @brief  Start python gui
 * @note   
 * @retval None
 */
void Gui_RunGui(void);

/**
 * @brief  Sand data to pythno gui via UDP socket
 * @note   
 * @param  data: pointer to data to send (best will be array)
 * @param  data_len: data lenght in bytes
 * @retval None
 */
void Gui_SendDataToPlot(void * data, uint16_t data_len);

#endif /* GUI_H */
