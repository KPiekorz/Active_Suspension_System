#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*** SYSTEM_FIFO_SIZE ***/

#define DEFAULT_FIFO_SIZE                   (256)

/*** SYSTEM MESSAGE OFFSETS ***/

#define SYSTEM_MESSAGE_TYPE_OFFSET          (0)
#define SYSTEM_MESSAGE_LENGTH_OFFSET        (1)
#define SYSTEM_MESSAGE_DATA_OFFSET          (2)
#define GET_MESSAGE_LEN(l)                  ((l + 2) * sizeof(float))

/*** TIME TRANSFORMATION ***/

#define SEC_TO_US(s)                        (s * 1000000)
#define MS_TO_US(s)                         (s * 1000)

/*** DELAY MACROS ***/

#define DELAY_S(s)                          (usleep(SEC_TO_US(s)))
#define DELAY_MS(s)                         (usleep(MS_TO_US(s)))

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

/*** SYSTEM FUNCTION ***/

/**
 * @brief  Send messages to other process created in system.
 * @note
 * @param  fifo_name: fifo queue name
 * @param  data: data
 * @param  data_len: data length
 * @retval TRUE if operation was successful, otherwise FALSE.
 */
bool SystemUtility_SendMessage(const char * fifo_name, int message_type, float * data, int data_len);

/**
 * @brief  Copy src float array , to dest float array.
 * @note
 * @param  src: source float array
 * @param  dest: destination float array
 * @param  len: length of of shorter array
 * @retval None
 */
void SystemUtility_CopyFloatArray(float * src, float * dest, int len);

#endif  /* SYSTEM_UTILITY_H */
