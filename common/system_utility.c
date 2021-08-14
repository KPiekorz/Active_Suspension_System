#include "system_utility.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>

#define FLOAT_SIZE (4)

/*** STATIC FUNCTION ***/

static void systemUtility_SetThreadPriority(pthread_t thread_id, system_thread_piority_t thread_priority)
{
    /* Scheduling policy: FIFO or RR (thsi is the only rtos scheduling) */
    int policy;

    /* Structure of other thread parameters */
    struct sched_param param;

    /* Read modify and set new thread priority */
    pthread_getschedparam(thread_id, &policy, &param);

    int max_priority = sched_get_priority_max(policy);
    int min_priority = sched_get_priority_min(policy);

    switch (thread_priority)
    {
        case thread_priority_min:
            param.sched_priority = sched_get_priority_min(policy);
        break;
        case thread_priority_low:
            param.sched_priority = (int)((float)((float)max_priority-(float)min_priority)*(float)0.25);
        break;
        case thread_priority_medium:
            param.sched_priority = (int)((float)((float)max_priority-(float)min_priority)*(float)(0.5));
        break;
        case thread_priority_high:
            param.sched_priority = (int)((float)((float)max_priority-(float)min_priority)*(float)0.75);
        break;
        case thread_priority_max:
            param.sched_priority = sched_get_priority_max(policy);
        break;
        default:
            DEBUG_LOG_ERROR("[SYSTEM] Invalid thread piority!")
        break;
    }

    // DEBUG_LOG_DEBUG("NUM PIOR: %d, PIOR VALUE: %d", thread_priority, param.sched_priority);

    pthread_setschedparam(thread_id, policy, &param);
}

/*** GLOBAL FUNCTION ***/

bool SystemUtility_CreateMessageFifo(const char *fifo_name)
{
    /* Create FIFO */
    if ((mkfifo(fifo_name, 0664) == -1) && (errno != EEXIST))
    {
        DEBUG_LOG_ERROR("[SYSTEM] Cannot create FIFO.");
        return false;
    }

    return true;
}

bool SystemUtility_SendMessage(const char *fifo_name, int message_type, float *float_data, const int float_data_len)
{
    // copy float to byte array and send byte array

    if (GET_FULL_MESSAGE_SIZE(float_data_len) > DEFAULT_FIFO_SIZE)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_SendMessage, Message too long, float data len: %d", float_data_len);
        return false;
    }

    int fd;
    int buff_len = GET_FULL_MESSAGE_SIZE(float_data_len);
    byte buff[buff_len];

    /* set metadata values of message */
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = (byte)message_type;
    buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET] = (byte)GET_FLOAT_DATA_SIZE(float_data_len);

    if (GET_FLOAT_DATA_SIZE(float_data_len) != SystemUtility_SetFloatArrayInByteArray(float_data, float_data_len, &(buff[SYSTEM_MESSAGE_PAYLOAD_OFFSET]), GET_FLOAT_DATA_SIZE(float_data_len)))
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_SendMessage, Can't set float array to byte array!");
        return false;
    }

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_WRONLY)) == -1)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_SendMessage, Can't open fifo!");
        return false;
    }

    bool result = true;

    /* Write a message to FIFO */
    if (write(fd, buff, buff_len) != buff_len)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_SendMessage, Can't write to fifo!");
        result = false;
    }

    /* Close FIFO */
    if (close(fd) == -1)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_SendMessage, Can't close fifo!");
        result = false;
    }

    return result;
}

bool SystemUtility_ReceiveMessage(const char *fifo_name, int *message_type, float *float_data, int *float_data_len)
{
    int fd, bytes_read;
    byte buff[DEFAULT_FIFO_SIZE];

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_RDONLY)) == -1)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_ReceiveMessage, Cannot open FIFO.");
        return false;
    }

    /* receive message from other modules form system */
    memset(buff, '\0', sizeof(buff));

    /* Read data from FIFO */
    if ((bytes_read = read(fd, buff, sizeof(buff))) == -1)
    {
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_ReceiveMessage, Something is wrong with FIFO.");
        return false;
    }

    /* If there is no data just continue */
    if (bytes_read == 0)
    {
        return false;
    }

    /* If there is message print it */
    if (bytes_read > 2)
    {
        if (GET_FLOAT_DATA_LEN(buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET]) > *float_data_len)
        {
            return false;
        }

        *message_type = (int)buff[SYSTEM_MESSAGE_TYPE_OFFSET];

        *float_data_len = (int)GET_FLOAT_DATA_LEN(buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET]);

        if (*float_data_len != SystemUtility_GetFloatArrayFromByteArray(&(buff[SYSTEM_MESSAGE_PAYLOAD_OFFSET]), buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET], float_data, (const int)*float_data_len))
        {
            return false;
        }
    }
    else
    {
        DEBUG_LOG_WARN("[SYSTEM] SystemUtility_ReceiveMessage, Message too short!!!");
        return false;
    }

    return true;
}

void SystemUtility_CopyFloatArray(float *src, float *dest, int len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

int SystemUtility_SetFloatArrayInByteArray(float *src, const int src_len, byte *dest, const int dest_len)
{
    if (src_len == 0 ||
        sizeof(unsigned int) != sizeof(float) ||
        FLOAT_SIZE != sizeof(float))
    {
        return 0;
    }

    if ((src_len * sizeof(float)) > dest_len)
    {
        return 0;
    }

    int byte_index = 0;

    for (int i = 0; i < src_len; i++)
    {
        // DEBUG_LOG_DEBUG("[SYSTEM] Src i: %f", src[i]);

        unsigned int asInt = *((unsigned int *)&(src[i]));

        // DEBUG_LOG_DEBUG("[SYSTEM] asInt: %d", asInt);

        dest[byte_index++] = (asInt)&0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt) & 0xFF);

        dest[byte_index++] = (asInt >> 8) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 8) & 0xFF);

        dest[byte_index++] = (asInt >> 16) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 16) & 0xFF);

        dest[byte_index++] = (asInt >> 24) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 24) & 0xFF);
    }

    return byte_index;
}

int SystemUtility_GetFloatArrayFromByteArray(byte *src, const int src_len, float *dest, const int dest_len)
{
    if (sizeof(unsigned int) != sizeof(float) ||
        FLOAT_SIZE != sizeof(float))
    {
        return 0;
    }

    if (src_len == 0)
    {
        return 0;
    }

    int float_array_len = src_len / (int)sizeof(float);

    if (float_array_len < 1)
    {
        return 0;
    }

    int byte_index = 0;
    for (int i = 0; i < float_array_len; i++)
    {
        unsigned int asInt = 0;
        asInt = (unsigned int)src[byte_index++];
        asInt |= ((unsigned int)src[byte_index++]) << 8;
        asInt |= ((unsigned int)src[byte_index++]) << 16;
        asInt |= ((unsigned int)src[byte_index++]) << 24;

        dest[i] = *((float *)&asInt);

        // DEBUG_LOG_DEBUG("[SYSTEM] Dest i: %f", dest[i]);
    }

    return float_array_len;
}

bool SystemUtility_CreateThread(void *(*__start_routine) (void *))
{
    int status;

    /* Thread variable */
    pthread_t tSimpleThread;

    /* Thread attributes variable */
    pthread_attr_t aSimpleThreadAttr;

    /* Initialize thread attributes structure for FIFO scheduling */
    pthread_attr_init(&aSimpleThreadAttr);
    pthread_attr_setschedpolicy(&aSimpleThreadAttr, SCHED_FIFO);

    /* Create thread */
    if ((status = pthread_create(&tSimpleThread, &aSimpleThreadAttr, __start_routine, NULL)))
    {
        return false;
    }

    return true;
}

bool SystemUtility_CreateCyclicThread(void *(*__start_routine) (void *), system_timer_t * timer, int interval_ms)
{
    int	status;

    /* Threads variables */
    pthread_t Thread;

    /* Threads attributes variables */
    pthread_attr_t ThreadAttr;

    /* Signal variable */
    struct	sigevent timerEvent;

    /* Initialize threads attributes structures for FIFO scheduling */
	pthread_attr_init(&ThreadAttr);
	pthread_attr_setschedpolicy(&ThreadAttr, SCHED_FIFO);

	/* Initialize event to send signal SIGRTMAX */
	timerEvent.sigev_notify = SIGEV_THREAD;
    timerEvent.sigev_notify_function = (void *)__start_routine;
	timerEvent.sigev_notify_attributes = &ThreadAttr;

	/* Create timer */
  	if ((status = timer_create(CLOCK_REALTIME, &timerEvent, &(timer->timerVar))))
    {
  		DEBUG_LOG_ERROR("SystemUtility_CreateCyclicThread, Error creating timer : %d", status);
  		return false;
  	}

    int sec = (int)((float)interval_ms / (float)1000);
    interval_ms -= (sec * 1000);
    long int nano_sec = ((long int)1000000 * (long int)interval_ms);

  	/* Set up timer structure with time parameters */
	timer->timerSpecStruct.it_value.tv_sec = sec;
	timer->timerSpecStruct.it_value.tv_nsec = nano_sec;
	timer->timerSpecStruct.it_interval.tv_sec = sec;
	timer->timerSpecStruct.it_interval.tv_nsec = nano_sec;

	/* Change timer parameters and run */
  	timer_settime( timer->timerVar, 0, &(timer->timerSpecStruct), NULL);

    return true;
}

void SystemUtility_DestroyCyclicThread(system_timer_t * timer)
{
    /* Set up timer structure with time parameters */
	timer->timerSpecStruct.it_value.tv_sec = 0;
	timer->timerSpecStruct.it_value.tv_nsec = 0;
	timer->timerSpecStruct.it_interval.tv_sec = 0;
	timer->timerSpecStruct.it_interval.tv_nsec = 0;

	/* Change timer parameters and stop */
  	timer_settime( timer->timerVar, 0, &(timer->timerSpecStruct), NULL);
}

void SystemUtility_InitThread(pthread_t thread_id, system_thread_piority_t thread_priority)
{
    systemUtility_SetThreadPriority(thread_id, thread_priority);
}

bool SystemUtility_CreateMQueue(const char * queue_name, int max_messge_num, size_t message_size)
{
    return true;
}

bool SystemUtility_SendMQueueMessage(const char * queue_name, void * message)
{
    return true;
}

bool SystemUtility_ReceiveMQueueMessage(const char * queue_name, void * message)
{
    return true;
}
