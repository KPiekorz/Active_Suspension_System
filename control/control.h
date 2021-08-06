#ifndef CONTROL_H
#define CONTROL_H

// #define INIT_CONTROL

typedef enum
{
    control_message_unknown = 0,
    control_message_model_states,
} control_message_type_t;

/**
 * @brief  Init control process
 * @note
 * @retval None
 */
void Control_Init(void);

/**
 * @brief  Destroy control process
 * @note
 * @retval None
 */
void Control_Destroy(void);

/**
 * @brief  Send fifo message to control message process
 * @note
 * @param  message_type: control message type
 * @param  *data: data to control process
 * @param  data_len: len of data to control process
 * @retval None
 */
void Control_SendMessage(control_message_type_t message_type, float *data, int data_len);

#endif /* CONTROL_H */
