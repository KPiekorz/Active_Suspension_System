#include "system_utility.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>

uint16_t SystemUtility_SetOneByte(uint8_t * buf, uint16_t byte_index, uint8_t value)
{
    buf[byte_index] = value;
    return 1;
}

uint16_t SystemUtility_SetTwoBytesLE(uint8_t * buf, uint16_t byte_index, uint16_t value)
{
    buf[byte_index] = value&0x00FF;
    buf[byte_index] = (value>>8)&0x00FF;
    return 2;
}

uint16_t SystemUtility_GetOneByte(uint8_t * buf, uint16_t byte_index, uint8_t value)
{

}

uint16_t SystemUtility_GetTwoBytesLE(uint8_t * buf, uint16_t byte_index, uint16_t value)
{

}

bool SystemUtility_SendMessage(const char * fifo_name, uint8_t message_type, uint8_t * data, uint16_t data_len)
{
    int fd;
    uint8_t buff[GET_MESSAGE_SIZE(data_len)];
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = message_type;
    buff[SYSTEM_MESSAGE_LENGTH_OFFSET] = (uint8_t)data_len;
    memcpy(&(buff[SYSTEM_MESSAGE_DATA_OFFSET]), data, data_len);

    /* Open FIFO file */
    if ((fd = open(fifo_name, O_WRONLY)) == -1)
    {
        return false;
    }

    /* Write a message to FIFO */
    if (write(fd, buff, GET_MESSAGE_SIZE(data_len)) != GET_MESSAGE_SIZE(data_len))
    {
        return false;
    }

    /* Close FIFO */
    if (close(fd) == -1)
    {
        return false;
    }
}