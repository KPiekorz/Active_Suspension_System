#include "system_utility.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>

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
    int byte_array_size = (int)sizeof(float) * src_len;

    if (src_len == 0 || byte_array_size > dest_len)
    {
        return 0;
    }

    if (sizeof(unsigned int) != sizeof(float))
    {
        return 0;
    }

    int byte_index = 0;
    for (int i = 0; i < src_len; i++)
    {
        unsigned int asInt = *((int*)&(src[i]));
        for (int j = 0; j < sizeof(float); j++)
        {
            dest[byte_index++] = (asInt >> (8 * j)) & 0xFF;
        }
    }

    return byte_array_size;
}

int SystemUtility_GetFloatArrayFromByteArray(byte * src, const int src_len, float * dest, const int dest_len)
{
    if (src_len == 0)
    {
        return 0;
    }

    if (sizeof(unsigned int) != sizeof(float))
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
        for (int j = 0; j < sizeof(float); j++)
        {
            asInt = (src[byte_index++] << (8 * j));
        }

        dest[i] = (float)asInt;
    }

    return float_array_size;
}