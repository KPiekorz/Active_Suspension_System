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

static void systemUtility_CopyFloatArray(float * src, float * dest, int len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

/*** GLOBAL FUNCTION ***/

bool SystemUtility_SendMessage(const char * fifo_name, int message_type, float * data, int data_len)
{
    int fd;
    float buff[GET_MESSAGE_LEN(data_len)];
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = (float)message_type;
    buff[SYSTEM_MESSAGE_LENGTH_OFFSET] = (float)data_len;
    systemUtility_CopyFloatArray(data, &(buff[SYSTEM_MESSAGE_DATA_OFFSET]), data_len);

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