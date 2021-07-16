#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/*** SYSTEM_FIFO_SIZE ***/

#define DEFAULT_FIFO_SIZE                           (256)

/*** SYSTEM MESSAGE OFFSETS (in bytes from array beginning) ***/

#define SYSTEM_MESSAGE_TYPE_OFFSET                  (0)
#define SYSTEM_MESSAGE_PAYLOAD_SIZE_OFFSET          (1)
#define SYSTEM_MESSAGE_PAYLOAD_OFFSET               (2)

#define GET_FLOAT_DATA_SIZE(float_data_len)         (float_data_len * sizeof(float)) // only float data size in bytes
#define GET_FULL_MESSAGE_SIZE(float_data_len)       (GET_FLOAT_DATA_SIZE(float_data_len) + 2) // float array size in bytes and message metadata size in bytes (message type and size of payload)

#define GET_FLOAT_DATA_LEN(message_payload_size)    (message_payload_size / sizeof(float))

/*** TIME TRANSFORMATION ***/

#define SEC_TO_US(s)                                (s * 1000000)
#define MS_TO_US(s)                                 (s * 1000)

/*** DELAY MACROS ***/

#define DELAY_S(s)                                  (usleep(SEC_TO_US(s)))
#define DELAY_MS(s)                                 (usleep(MS_TO_US(s)))

/*** DEBUG LOGS ***/

#define ENABLE_VERBOSE_LOG
#define ENABLE_INFO_LOG
#define ENABLE_DEBUG_LOG
#define ENABLE_WARN_LOG

#define DEBUG_LOG_ALWAYS(...)       { printf("ALWAYS: " __VA_ARGS__);   printf("\n"); }

#ifdef ENABLE_INFO_LOG
    #define DEBUG_LOG_INFO(...)     { printf("INFO: " __VA_ARGS__);     printf("\n"); }
#else
    #define DEBUG_LOG_INFO(...)    // nothing
#endif

#ifdef ENABLE_DEBUG_LOG
    #define DEBUG_LOG_DEBUG(...)    { printf("DEBUG: " __VA_ARGS__);    fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_DEBUG(...)    // nothing
#endif

#ifdef ENABLE_VERBOSE_LOG
    #define DEBUG_LOG_VERBOSE(...)  { printf("VERBOSE: " __VA_ARGS__);  fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_VERBOSE(...)  // nothing
#endif

#ifdef ENABLE_WARN_LOG
    #define DEBUG_LOG_WARN(...)     { printf("WARN: " __VA_ARGS__);     fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_WARN(...)     // nothing
#endif

#define DEBUG_LOG_ERROR(...)        { fprintf(stderr, "ERROR: " __VA_ARGS__);    printf("\n"); }

/*** BYTE TYPE ***/

typedef uint8_t byte; //unsigned int //char // uint8_t

/*** SYSTEM FUNCTION ***/

/**
 * @brief  Create message fifo queue
 * @note
 * @param  fifo_name: fifo name
 * @retval true if successfully queue created, otherwise false
 */
bool SystemUtility_CreateMessageFifo(const char * fifo_name);

/**
 * @brief  Send messages to other process created in system.
 * @note
 * @param  fifo_name: fifo queue name
 * @param  float_data: data (length of float array)
 * @param  float_data_len: data length
 * @retval true if successfully send message, otherwise false
 */
bool SystemUtility_SendMessage(const char * fifo_name, int message_type, float * float_data, const int float_data_len);

/**
 * @brief  Receive message from other process created in system
 * @note
 * @param  fifo_name: fifo queue name
 * @param  message_type: [out] message type
 * @param  float_data: [out] float data
 * @param  float_data_len: [out] data length of received float message (as input you have to provide max float data array length)
 * @retval true if successfully received data, otherwise false
 */
bool SystemUtility_ReceiveMessage(const char * fifo_name, int * message_type, float * float_data, int * float_data_len);

/**
 * @brief  Copy src float array , to dest float array.
 * @note
 * @param  src: source float array
 * @param  dest: destination float array
 * @param  len: length of of shorter array
 * @retval None
 */
void SystemUtility_CopyFloatArray(float * src, float * dest, int len);

/**
 * @brief  Set foat array in byte array
 * @note
 * @param  src: float array
 * @param  src_len: float aray length
 * @param  dest: byte array
 * @param  dest_len: dest array max length
 * @retval Size of used byte array.
 */
int SystemUtility_SetFloatArrayInByteArray(float * src, const int src_len, byte * dest, const int dest_len);

/**
 * @brief  Get float array from byte array
 * @note
 * @param  src: byte array
 * @param  src_len: size of byte array
 * @param  dest: float array
 * @param  dest_len: float array max length
 * @retval Size of used float array.
 */
int SystemUtility_GetFloatArrayFromByteArray(byte * src, const int src_len, float * dest, const int dest_len);

/**
 * @brief  Create thread
 * @note
 * @param  __start_routine: thread function
 * @retval true if successfully created thread, otherwise false.
 */
bool SystemUtility_CreateThread(void *(*__start_routine) (void *));

/**
 * @brief  Init thread
 * @note
 * @param  thread_id: thread id
 * @retval None
 */
void SystemUtility_InitThread(pthread_t thread_id);

#endif  /* SYSTEM_UTILITY_H */
