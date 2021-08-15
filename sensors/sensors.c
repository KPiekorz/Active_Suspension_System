/*
* Senssor process is for operate sensors, which are mounted on car. Now for simulation, model states will be used as sensor output for control algoritm.
*/

#include "sensors.h"
#include <stdio.h>

#include "gui.h"
#include "system_utility.h"

/*** message queue for receive message ***/

#define MAX_SENSOR_MQUEUE_LEN	10

const struct mq_attr myMQueueAttr = {.mq_maxmsg = MAX_SENSOR_MQUEUE_LEN, .mq_msgsize = sizeof(sensor_mqueue_message_t)};
const system_mqueue_t sensor_mqueue = {.myMQueueAttr = &myMQueueAttr, .queue_name = "/sensor_queue"};
#define GetSensorMQueue()	(&sensor_mqueue)

/*** STATIC FUNCTION ***/

static void *sensors_ReceiveMessageThread(void *cookie)
{
	/* init sensor receive thread */
	SystemUtility_InitThread(pthread_self(), thread_priority_high);

	/* mqueu mesage to sensor process */
	sensor_mqueue_message_t message;

	while(1)
	{
		if (SystemUtility_ReceiveMQueueMessage(GetSensorMQueue(), &message, sizeof(message)))
		{
			// DEBUG_LOG_DEBUG("SENSOR MSG: %f, %f, %f, %f", message.state_X1, message.state_X1_dot, message.state_Y1, message.state_Y1_dot);
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

	/* remove sensor message queeu */
	SystemUtility_RemoveMQueue(GetSensorMQueue());

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

	DEBUG_LOG_INFO("SENSOR CLOSED!");

	SystemUtility_RemoveMQueue(GetSensorMQueue());
}

void Sensor_SendMQueueMessage(sensor_mqueue_message_t * message,  size_t message_size)
{
    if (!SystemUtility_SendMQueueMessage(GetSensorMQueue(), message, message_size))
    {
        DEBUG_LOG_ERROR("[SENSOR] Sensor_SendMQueueMessage, Can't send mqueue message sensor process!");
    }
}