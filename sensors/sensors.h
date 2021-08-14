#ifndef SENSORS_H
#define SENSORS_H

#include <stdlib.h>

#define INIT_SENSORS

/*** sensor mqueue message ***/

typedef struct
{
	float state_X1;
} sensor_mqueue_message_t;

/**
 * @brief  Init sensor process
 * @note
 * @retval None
 */
void Sensor_Init(void);

/**
 * @brief  Destroy sensor process
 * @note
 * @retval None
 */
void Sensor_Destroy(void);

/**
 * @brief  Send mqueue to sensor process
 * @note
 * @param  message: message
 * @retval None
 */
void Sensor_SendMQueueMessage(sensor_mqueue_message_t * message, size_t message_size);

#endif /* SENSORS_H */