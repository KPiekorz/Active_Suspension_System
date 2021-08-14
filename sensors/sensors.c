#include "sensors.h"
#include <stdio.h>

#include "gui.h"
#include "system_utility.h"

/*** message queue for receive message ***/

system_mqueue_t sensor_mqueue;
#define GetSensorMQueue()	(&sensor_mqueue)

const char * sensor_queue_name = "/sensor_queue";

#define MAX_SENSOR_MQUEUE_LEN	10

/*** STATIC FUNCTION ***/

static void *sensors_ReceiveMessageThread(void *cookie)
{
	SystemUtility_InitThread(pthread_self(), thread_priority_high);

	sensor_mqueue_message_t message;

	while(1)
	{
		if (SystemUtility_ReceiveMQueueMessage(GetSensorMQueue(), &message))
		{
			DEBUG_LOG_DEBUG("SENSOR MSG: %f", message.state_X1);
		}
		else
		{
			DEBUG_LOG_ERROR("[SENSOR] sensors_ReceiveMessageThread, Error when receive message from mqueue!");
		}
	}

	return 0;
}

/*** GLOBAL FUNCTION ***/

void Sensor_Init(void)
{
#ifdef INIT_SENSORS

	/* create mqueue for receiving messages */
	if (!SystemUtility_CreateMQueue(GetSensorMQueue(), sensor_queue_name, MAX_SENSOR_MQUEUE_LEN, sizeof(sensor_mqueue_message_t)))
	{
        DEBUG_LOG_ERROR("[SENSOR] Sensor_Init, Can't create mqueue!");
	}

    /* Init receive message */
	if (!SystemUtility_CreateThread(sensors_ReceiveMessageThread))
    {
        DEBUG_LOG_ERROR("[SENSOR] Sensor_Init, Can't create receive thread!");
    }

    while (1)
    {
        DELAY_S(SYSTEM_EVENT_LOOP_DELAY_S);
    }

#endif /* INIT_SENSORS */
}

void Sensor_Destroy(void)
{

}

void Sensor_SendMQueueMessage(sensor_mqueue_message_t * message)
{
    if (!SystemUtility_SendMQueueMessage(GetSensorMQueue(), message))
    {
        DEBUG_LOG_ERROR("[SENSOR] Sensor_SendMQueueMessage, Can't send mqueue message sensor process!");
    }
}