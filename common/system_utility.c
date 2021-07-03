#include "system_utility.h"

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