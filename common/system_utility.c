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

bool SystemUtility_SendMessage(const char * fifo_name, int message_type, float * data, int data_len)
{
    int fd;
    float buff[GET_MESSAGE_LEN(data_len)];
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = (float)message_type;
    buff[SYSTEM_MESSAGE_LENGTH_OFFSET] = (float)data_len;
    SystemUtility_CopyFloatArray(data, &(buff[SYSTEM_MESSAGE_DATA_OFFSET]), data_len);

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_WRONLY)) == -1)
    {
        return false;
    }

    /* Write a message to FIFO */
    if (write(fd, buff, GET_MESSAGE_LEN(data_len)) != GET_MESSAGE_LEN(data_len))
    {
        return false;
    }

    /* Close FIFO */
    if (close(fd) == -1)
    {
        return false;
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
    if (sizeof(unsigned int) != sizeof(float) ||
        FLOAT_SIZE != sizeof(float))
    {
        return 0;
    }

    int byte_array_size = (int)sizeof(float) * src_len;

    if (src_len == 0 || byte_array_size > dest_len)
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
        DEBUG_LOG_DEBUG("Byte: %d", (asInt) & 0xFF);

        dest[byte_index++] = (asInt >> 8) & 0xFF;
        DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 8) & 0xFF);

        dest[byte_index++] = (asInt >> 16) & 0xFF;
        DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 16) & 0xFF);

        dest[byte_index++] = (asInt >> 24) & 0xFF;
        DEBUG_LOG_DEBUG("Byte: %d", (asInt >> 24) & 0xFF);

    }

    return byte_array_size;
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