#include "system_utility.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>

#define FLOAT_SIZE (4)

/*** STATIC FUNCTION ***/

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
        DEBUG_LOG_ERROR("[SYSTEM] Cannot open FIFO.");
        return false;
    }

    /* receive message from other modules form system */
    memset(buff, '\0', sizeof(buff));

    /* Read data from FIFO */
    if ((bytes_read = read(fd, buff, sizeof(buff))) == -1)
    {
        DEBUG_LOG_ERROR("[SYSTEM] Something is wrong with FIFO.");
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
        DEBUG_LOG_DEBUG("[SYSTEM] SystemUtility_ReceiveMessage, type: %d, len: %d, float data len: %d",
                        buff[SYSTEM_MESSAGE_TYPE_OFFSET],
                        buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET],
                        *float_data_len);

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
        DEBUG_LOG_ERROR("[SYSTEM] SystemUtility_CreateThread, Can't create a thread!");
        return false;
    }

    return true;
}

void SystemUtility_InitThread(pthread_t thread_id)
{
    /* Scheduling policy: FIFO or RR */
    int policy;
    /* Structure of other thread parameters */
    struct sched_param param;

    /* Read modify and set new thread priority */
    pthread_getschedparam(thread_id, &policy, &param);
    param.sched_priority = sched_get_priority_max(policy);
    pthread_setschedparam(thread_id, policy, &param);
}