#include "system_utility.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>

#define FLOAT_SIZE      (4)

/*** STATIC FUNCTION ***/



/*** GLOBAL FUNCTION ***/

int SystemUtility_SendMessage(const char * fifo_name, int message_type, float * float_data, const int float_data_len)
{
    // copy float to byte array and send byte array

    if (GET_FULL_MESSAGE_SIZE(float_data_len) > DEFAULT_FIFO_SIZE)
    {
        DEBUG_LOG_ERROR("SystemUtility_SendMessage, Message too long, float data len: %d", float_data_len);
        return 0;
    }

    int fd;
    int buff_len = GET_FULL_MESSAGE_SIZE(float_data_len);
    byte buff[buff_len];

    /* set metadata values of message */
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = (byte)message_type;
    buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET] = (byte)GET_FLOAT_DATA_SIZE(float_data_len);

    if (GET_FLOAT_DATA_SIZE(float_data_len) != SystemUtility_SetFloatArrayInByteArray(float_data, float_data_len, &(buff[SYSTEM_MESSAGE_PAYLOAD_OFFSET]), GET_FLOAT_DATA_SIZE(float_data_len)))
    {
        DEBUG_LOG_ERROR("SystemUtility_SendMessage, Can't set float array to byte array!");
        return 0;
    }

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_WRONLY)) == -1)
    {
        DEBUG_LOG_ERROR("SystemUtility_SendMessage, Can't open fifo!");
        return 0;
    }

    int float_data_len_send = float_data_len;

    /* Write a message to FIFO */
    if (write(fd, buff, GET_FULL_MESSAGE_SIZE(float_data_len)) != GET_FULL_MESSAGE_SIZE(float_data_len))
    {
        DEBUG_LOG_ERROR("SystemUtility_SendMessage, Can't write to fifo!");
        float_data_len_send = 0;
    }

    /* Close FIFO */
    if (close(fd) == -1)
    {
        DEBUG_LOG_ERROR("SystemUtility_SendMessage, Can't close fifo!");
        float_data_len_send = 0;
    }

    return float_data_len_send;
}

int SystemUtility_ReceiveMessage(const char * fifo_name, int message_type, float * data, int data_len)
{
    int fd, bytes_read;
    byte buff[DEFAULT_FIFO_SIZE];

    /* Create FIFO */
    if ((mkfifo(fifo_name, 0664) == -1) && (errno != EEXIST))
    {
        DEBUG_LOG_ERROR("[GUI] Cannot create FIFO.");
        return 0;
    }

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_RDONLY)) == -1)
    {
        DEBUG_LOG_ERROR("[GUI] Cannot open FIFO.");
        return 0;
    }

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] Waiting for new message.");

        /* receive message from other modules form system */
        memset(buff, '\0', sizeof(buff));

        /* Read data from FIFO */
        if ((bytes_read = read(fd, buff, sizeof(buff))) == -1)
        {
            DEBUG_LOG_ERROR("[GUI] Something is wrong with FIFO.");
            return 0;
        }

        /* If there is no data just continue */
        if (bytes_read == 0)
        {
            DELAY_MS(1000);
            continue;
        }

        /* If there is message print it */
        if (bytes_read > 2)
        {
            DEBUG_LOG_DEBUG("[GUI] gui_ReceiveMessageThread, type: %d, len: %d",
                            buff[SYSTEM_MESSAGE_TYPE_OFFSET],
                            buff[SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET]);

            byte * data = &(buff[SYSTEM_MESSAGE_PAYLOAD_OFFSET]);

            DEBUG_LOG_DEBUG("[GUI] Value: %d", data[0]);
        }
        else
        {
            DEBUG_LOG_WARN("[GUI] gui_ReceiveMessageThread, Message too short!!!");
        }
    }
}

void SystemUtility_CopyFloatArray(float * src, float * dest, int len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

int SystemUtility_SetFloatArrayInByteArray(float * src, const int src_len, byte * dest, const int dest_len)
{
    if (src_len == 0 ||
        sizeof(unsigned int) != sizeof(float) ||
        FLOAT_SIZE != sizeof(float))
    {
        return 0;
    }

    int byte_index = 0;

    for (int i = 0; i < src_len; i++)
    {
        DEBUG_LOG_DEBUG("Src i: %f", src[i]);

        unsigned int asInt = *((unsigned int*)&(src[i]));
        DEBUG_LOG_DEBUG("asInt: %d", asInt);

        dest[byte_index++] = (asInt) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt) & 0xFF);

        dest[byte_index++] = (asInt >> 8) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 8) & 0xFF);

        dest[byte_index++] = (asInt >> 16) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 16) & 0xFF);

        dest[byte_index++] = (asInt >> 24) & 0xFF;
        // DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 24) & 0xFF);

    }

    return dest_len;
}

int SystemUtility_GetFloatArrayFromByteArray(byte * src, const int src_len, float * dest, const int dest_len)
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

    int float_array_size = src_len / (int)sizeof(float);

    if (float_array_size < 1)
    {
        return 0;
    }

    int byte_index = 0;
    for (int i = 0; i < float_array_size; i++)
    {
        unsigned int asInt = 0;
        asInt = (unsigned int)src[byte_index++];
        asInt |= ((unsigned int)src[byte_index++]) << 8;
        asInt |= ((unsigned int)src[byte_index++]) << 16;
        asInt |= ((unsigned int)src[byte_index++]) << 24;

        dest[i] = *((float*)&asInt);

        DEBUG_LOG_DEBUG("Dest i: %f", dest[i]);
    }

    return float_array_size;
}